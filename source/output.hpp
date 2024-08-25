#ifndef HX_OUTPUT_H
#define HX_OUTPUT_H

#include "editor.hpp"
#include "hx3dit.hpp"

namespace HX3 
{
// ---------------------------------------------------------------------------- 
    constexpr int32_t OutputBufferHeight = 512;
    constexpr int32_t OutputBufferWidth = 512;

    namespace UI 
    {
// ---------------------------------------------------------------------------- 
/**
 * Describes how content of UI element should be treated
 **/
// ---------------------------------------------------------------------------- 
        class IContent 
        {
            friend HX3::Output;

            bool m_bReset = false;
            UI::UiElement* m_pParent = nullptr;

        public:
            IContent() = default;
            IContent(UI::UiElement* pParent) : m_pParent(pParent) {};
            IContent(IContent &&) = default;
            IContent(const IContent &) = default;
            IContent &operator=(IContent &&) = default;
            IContent &operator=(const IContent &) = default;
            ~IContent() = default;
    
        public:
            void SetParent(UI::UiElement* pParent) { m_pParent = pParent; }

        public:
            virtual TerminalCell GetData(const uint32_t& uW, const uint32_t& uH) = 0;

            virtual void SetData(HX3::Byte* pData, uint64_t uSize) = 0;

        private:
            void Reset() { m_bReset = true; }

        protected:
            UI::UiElement* GetParentElement() { return m_pParent; }
            
            uint32_t GetWidth();

            uint32_t GetHeight();

            const bool& HasBeenReset() 
            { 
                // As most of the checks are going to be equal to false,
                // I want to return reference to not copy variables as much as possible.
                // My idea is to return a static true value when the IContent has
                // been reset. 
                if (m_bReset) 
                {
                    // This member is going to be set to false anyway.
                    m_bReset = false;

                    static bool r = true;
                    return r;
                }
                return m_bReset;
            }
        };

// ---------------------------------------------------------------------------- 
        class Spacer : public HX3::UI::IContent 
        {
            HX3::TerminalCell m_data = ' ';

        public:
            Spacer() = default;
            Spacer(HX3::TerminalCell content) : m_data(content) {};

        public:
            /**
             * This type of content always returns the same result
             **/
            virtual TerminalCell GetData(
                    const uint32_t&,
                    const uint32_t&) override 
            {
                return  m_data; 
            }

            virtual void SetData(HX3::Byte* pData, uint64_t which) override 
            {
                m_data = pData[which];
            }
        };

// ---------------------------------------------------------------------------- 
        class Text : public HX3::UI::IContent 
        {
            HX3::TerminalCell* m_data = nullptr;

        public:
            Text() = default;
            Text(HX3::TerminalCell* content) : m_data(content) {};

        public:
            /**
             * This type of content always returns the same result
             **/
            virtual TerminalCell GetData(
                    const uint32_t&,
                    const uint32_t&) override 
            {
                static uint32_t counter = 0;

                if (this->HasBeenReset())
                    counter = 0;

                return  m_data[counter++]; 
            }

            virtual void SetData(HX3::Byte* pData, uint64_t) override 
            {
                m_data = pData;
            }
        };

// ---------------------------------------------------------------------------- 
        enum DataRepresentation
        {
            Hex,
            Oct,
            Dec
        };

// ---------------------------------------------------------------------------- 
        constexpr uint8_t DataRepresentationSize = 3;

// ---------------------------------------------------------------------------- 
        class DataViewer : public HX3::UI::IContent 
        {
            HX3::Byte* m_pData;
            uint64_t m_uDataSize = 0;
            uint64_t m_uDataStartingPos = 0;
    
            DataRepresentation m_Representation = Dec;

        public:
            DataViewer() = default;
            DataViewer(HX3::UI::UiElement* pParent) : IContent(pParent) {};
            DataViewer(HX3::Byte* pData, uint64_t uSize) 
                : m_pData(pData),
                m_uDataSize(uSize)
            {};

        public:
            virtual TerminalCell GetData(
                    const uint32_t& uW, 
                    const uint32_t& uH) override;

            const HX3::UI::DataRepresentation& GetCurrentMode() 
            {
                return m_Representation; 
            }

        public:
            virtual void SetData(HX3::Byte* pData, uint64_t uSize) override 
            {
                m_pData = pData;
                m_uDataSize = uSize;
            }

            void SetMode(HX3::UI::DataRepresentation mode) 
            { 
                m_Representation = mode;
            }

        public:
            void UpdateData(HX3::Byte* pData, uint64_t uDataSize) 
            { 
                m_pData = pData; 
                m_uDataSize = std::move(uDataSize);
            }

        private:
            TerminalCell GetDec(
                    const uint32_t& uW,
                    const uint32_t& uH,
                    const uint32_t& uWidthLeft);

            TerminalCell GetHex(
                    const uint32_t& uW,
                    const uint32_t& uH,
                    const uint32_t& uWidthLeft);

            TerminalCell GetOct(
                    const uint32_t& uW,
                    const uint32_t& uH,
                    const uint32_t& uWidthLeft);

            std::string ConvertToHex(uint8_t v);

            std::string ConvertToOct(uint8_t v);

            uint8_t CalcWordsPerLine(const uint8_t& uWordWidth);

        };

// ---------------------------------------------------------------------------- 
        template<typename T>
            struct Dimensions 
            {
                T Left;
                T Bottom;
                T Top;
                T Right;
            };

// ---------------------------------------------------------------------------- 
        struct UiDimensions 
        {
            friend HX3::UserControls;
            friend HX3::UI::IContent;
            friend HX3::Output;

            union value 
            {
                Dimensions<float> Percent;
                Dimensions<int32_t> Cells;
            };

            bool UsePercentage = false;
            value Defaults = { 0, 0, 0, 0 };

            private:
            Dimensions<int32_t> CalculatedValues = {};
        };

// ---------------------------------------------------------------------------- 
        struct UiElement 
        {
            int64_t Index = 0;
            UiDimensions Dimensions = {};
            uint64_t Flags = 0;
            std::shared_ptr<IContent> Content = {};
        };

// ---------------------------------------------------------------------------- 
        enum UiFlags 
        {
            AlwaysVisible = 1,
        };

// ---------------------------------------------------------------------------- 
        class UiDataViewer : public HX3::UI::UiElement 
        {
        public: 
            UiDataViewer() { Content = std::make_shared<DataViewer>(this); }

            void SetData(HX3::Byte* pData, uint64_t uSize) 
            {
                reinterpret_cast<DataViewer*>(this->Content.get())->SetData(
                        pData,
                        uSize);
            }
        };

// ---------------------------------------------------------------------------- 
        class UiSpacer : public UiElement 
        {
            public:
                UiSpacer() { Content = std::make_shared<Spacer>(); }

                void SetData(HX3::Byte* pData, uint64_t uSize) 
                {
                    reinterpret_cast<Spacer*>(this->Content.get())->SetData(
                            pData,
                            uSize);
                }
        };

// ---------------------------------------------------------------------------- 
        class UiText : public UiElement 
        {
            public:
                UiText() { Content = std::make_shared<UI::Text>(); }

                void SetData(HX3::Byte* pData, uint64_t uSize) 
                {
                    reinterpret_cast<Text*>(this->Content.get())->SetData(
                            pData,
                            uSize);
                }
        };
    }

// ---------------------------------------------------------------------------- 
    struct SwappableBuffers 
    {
        SwappableBuffers() = delete;
        SwappableBuffers(int32_t h, int32_t w);
        ~SwappableBuffers();

        int32_t Width = 0;
        int32_t Height = 0;
        
        TerminalCell** Front = nullptr;
        TerminalCell** Back = nullptr;
        
        void Swap() 
        {
            TerminalCell** tmp = this->Front;
            this->Front = this->Back;
            this->Back = tmp;
        }
    };

// ---------------------------------------------------------------------------- 
    typedef std::vector<std::shared_ptr<HX3::UI::UiElement>> UIList;

// ---------------------------------------------------------------------------- 
    class Output 
    {
        HX3::SwappableBuffers m_Buffers = 
            HX3::SwappableBuffers(OutputBufferHeight, OutputBufferWidth);
    
        HX3::Vec2<uint64_t> m_uCurWindowDim = {};

        UIList m_UI = {};

        std::thread* m_pThread = nullptr;
        std::atomic_bool m_IsWorking = false;

        std::mutex m_UpdateLock = {};
            
    public:
        Output() = default;
        Output(Output &&) = delete;
        Output(const Output &) = delete;
        Output &operator=(Output &&) = delete;
        Output &operator=(const Output &) = delete;
        ~Output();

    public:
        void Initialize();
        
        void Destroy();

        void AddElement(std::shared_ptr<HX3::UI::UiElement> element);

        void Update();

    public:
        const uint64_t& GetWinWidth() { return m_uCurWindowDim.x; }
        const uint64_t& GetWinHeight() { return m_uCurWindowDim.y; }

    private:
        void LoopPrint();
    
        void ClearBackBuffer();
        void Print();
    };
}

#endif // !HX_OUTPUT_H
