#ifndef HX_EDITOR_H
#define HX_EDITOR_H

#include "hx3dit.hpp"

namespace HX3 
{
// ----------------------------------------------------------------------------   
    class Editor 
    {
        HX3::Byte* m_pRawLoadedFile = nullptr;

        uint64_t m_uFileSize = 0;
        uint64_t m_uFileCurrentPos = 0;

        std::atomic_bool m_Exit = false;

    public:
        Editor() = default;
        Editor(Editor &&) = delete;
        Editor(const Editor &) = delete;
        Editor &operator=(Editor &&) = delete;
        Editor &operator=(const Editor &) = delete;
        ~Editor();
       
    public:
        void SetFile(const char* szPath);

        void SetStatusExit() { m_Exit.store(true); }

    public:
        const uint64_t& GetCurPos() { return m_uFileCurrentPos; }
        const uint64_t& GetFileSize() { return m_uFileSize; }

        HX3::Byte* GetPtrToPos(const uint64_t& uPos);
        
        HX3::Byte* GetPtrToCurPos() 
        {
            return this->GetPtrToPos(this->GetCurPos());
        }

        bool GetStatus()  { return !m_Exit.load(); }
        
    public:
        void MovePos(const int64_t& distance);

    private:
        void ReleaseFileData();

    };
}

#endif // !HX_EDITOR_H
