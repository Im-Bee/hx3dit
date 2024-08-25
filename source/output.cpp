#include "hx3dit.hpp"
#include "editor.hpp"

#include "output.hpp"

using namespace std;

// ---------------------------------------------------------------------------- 
uint32_t HX3::UI::IContent::GetWidth()
{
    return this->m_pParent->Dimensions.CalculatedValues.Right -
        this->m_pParent->Dimensions.CalculatedValues.Left;
}

// ---------------------------------------------------------------------------- 
uint32_t HX3::UI::IContent::GetHeight()
{
    return this->m_pParent->Dimensions.CalculatedValues.Bottom -
        this->m_pParent->Dimensions.CalculatedValues.Top;
}

// ---------------------------------------------------------------------------- 
HX3::TerminalCell HX3::UI::DataViewer::GetData(
        const uint32_t& uW, 
        const uint32_t& uH) 
{
    uint32_t uWidthLeft = this->GetWidth();

    switch (m_Representation)
    {
        case UI::DataRepresentation::Hex:
            return this->GetHex(
                    uW,
                    uH,
                    uWidthLeft);

        case UI::DataRepresentation::Oct:
            return this->GetOct(
                    uW,
                    uH,
                    uWidthLeft);

        case UI::DataRepresentation::Dec:
            return this->GetDec(
                    uW,
                    uH,
                    uWidthLeft);

        default:
            throw;
    }
}

// ---------------------------------------------------------------------------- 
HX3::TerminalCell HX3::UI::DataViewer::GetDec(
        const uint32_t&, 
        const uint32_t&,
        const uint32_t&) 
{
    using std::pow;

    static uint64_t uCurData = 0;
    static uint64_t uCurChar = 0;
    static uint8_t& uNumber = this->m_pData[uCurData];
    constexpr uint8_t uWordWidth = 3;

    if (this->HasBeenReset())
    {
        uCurData = 0;
        uCurChar = 0;
        uNumber = this->m_pData[uCurData];
    }
    // We printed a whole word
    if (uCurChar >= uWordWidth)
    {
        if (++uCurData >= this->m_uDataSize) uNumber = 0;
        else uNumber = this->m_pData[uCurData];

        uCurChar = 0;
        return ' ';
    }
    
    return ((HX3::TerminalCell)
        (uNumber / pow(10, (uWordWidth  - (uCurChar++) - 1))) % 10) + AsciiZero;
}

// ---------------------------------------------------------------------------- 
HX3::TerminalCell HX3::UI::DataViewer::GetHex(
        const uint32_t&, 
        const uint32_t&,
        const uint32_t&) 
{
    using std::pow;

    static uint64_t uCurData = 0;
    static uint64_t uCurChar = 0;
    static std::string uNumber = ConvertToHex(this->m_pData[uCurData]);
    constexpr uint8_t uWordWidth = 2;

    if (this->HasBeenReset())
    {
        uCurData = 0;
        uCurChar = 0;
        uNumber = ConvertToHex(this->m_pData[uCurData]);
    }
    // We printed a whole word
    if (uCurChar >= uWordWidth)
    {
        if (++uCurData >= this->m_uDataSize) uNumber = "00";
        else uNumber = ConvertToHex(this->m_pData[uCurData]);

        uCurChar = 0;
        return ' ';
    }
    
    return uNumber[uCurChar++];
}

// ---------------------------------------------------------------------------- 
HX3::TerminalCell HX3::UI::DataViewer::GetOct(
        const uint32_t&, 
        const uint32_t&,
        const uint32_t&) 
{
    using std::pow;

    static uint64_t uCurData = 0;
    static uint64_t uCurChar = 0;
    static std::string uNumber = ConvertToOct(this->m_pData[uCurData]);
    constexpr uint8_t uWordWidth = 3;

    if (this->HasBeenReset())
    {
        uCurData = 0;
        uCurChar = 0;
        uNumber = ConvertToOct(this->m_pData[uCurData]);
    }
    // We printed a whole word
    if (uCurChar >= uWordWidth)
    {
        if (++uCurData >= this->m_uDataSize) uNumber = "000";
        else uNumber = ConvertToOct(this->m_pData[uCurData]);

        uCurChar = 0;
        return ' ';
    }
    
    return uNumber[uCurChar++];
}

// ---------------------------------------------------------------------------- 
uint8_t HX3::UI::DataViewer::CalcWordsPerLine(const uint8_t& uWordWidth)
{
    return (this->GetWidth() / uWordWidth);
}

// ---------------------------------------------------------------------------- 
std::string HX3::UI::DataViewer::ConvertToHex(uint8_t v)
{
    using std::pow;
    constexpr uint8_t base = 16;

    std::string result = "";
    static char glifs[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F'
    };

    uint8_t calc, myPow;
    uint8_t i = 1;
    while (result.size() != 2)
    {
        myPow = pow(base, i--);
        calc = v / myPow;
        v -= (calc * myPow);
        if (calc >= sizeof(glifs))
        {
            result += 'X';
            continue;
        }

        result += glifs[calc];
    }

    return result;
}

// ---------------------------------------------------------------------------- 
std::string HX3::UI::DataViewer::ConvertToOct(uint8_t v)
{
    using std::pow;
    constexpr uint8_t base = 8;

    std::string result = "";
    static char glifs[] = {
        '0', '1', '2', '3', '4', '5', '6', '7' 
    };

    uint8_t calc, myPow;
    uint8_t i = 2;
    while (result.size() != 3)
    {
        myPow = pow(base, i--);
        calc = v / myPow;
        v -= (calc * myPow);
        if (calc >= sizeof(glifs))
        {
            result += 'X';
            continue;
        }

        result += glifs[calc];
    }

    return result;
}

// ---------------------------------------------------------------------------- 
HX3::SwappableBuffers::SwappableBuffers(int32_t h, int32_t w) 
{
    this->Front = reinterpret_cast<TerminalCell**>(
            malloc(sizeof(TerminalCell*) * h));
    this->Back = reinterpret_cast<TerminalCell**>(
            malloc(sizeof(TerminalCell*) * h));

    for (int32_t i = 0; i < h; ++i) 
    {
        this->Front[i] = reinterpret_cast<TerminalCell*>(
                malloc(sizeof(TerminalCell) * w));
        this->Back[i] = reinterpret_cast<TerminalCell*>(
                malloc(sizeof(TerminalCell) * w));

        if (!this->Front[i] ||
            !this->Back[i])
            throw std::bad_alloc();

        for (int32_t k = 0; k < w; ++k) 
        {
            this->Front[i][k] = ' ';
            this->Back[i][k] = ' ';
        }
    }

    this->Height = h;
    this->Width = w;
}

// ---------------------------------------------------------------------------- 
HX3::SwappableBuffers::~SwappableBuffers() 
{
    for (int32_t i = 0; i < this->Height; ++i) 
    {
        delete[] this->Front[i];
        delete[] this->Back[i];
    }
}

// ---------------------------------------------------------------------------- 
HX3::Output::~Output() 
{
    this->Destroy();
}

// ---------------------------------------------------------------------------- 
void HX3::Output::Initialize() 
{
    HX_DBG_PRT_N("Initializing output thread");

    if (m_IsWorking.load() || m_pThread) 
        throw runtime_error("Cannot initialize same input second time");

    m_IsWorking = true;
    m_pThread = new thread(&HX3::Output::LoopPrint, this);
}

// ---------------------------------------------------------------------------- 
void HX3::Output::Destroy() 
{
    HX_DBG_PRT_N("Killing output thread");

    if (m_pThread)
    {
        m_IsWorking.store(false);
        m_pThread->join();
        m_pThread = nullptr;
    }
}

// ---------------------------------------------------------------------------- 
void HX3::Output::AddElement(std::shared_ptr<HX3::UI::UiElement> element) 
{
    for (size_t i = 0; i < m_UI.size(); ++i) 
    {
        if (m_UI[i]->Index <= element->Index)
            continue;
        
        m_UI.insert(m_UI.begin() + i, element);

#ifndef _DEBUG
        return;
#else
        goto debug;
#endif // !_DEBUG
    }

    m_UI.push_back(element);

#ifdef _DEBUG
debug:
    std::cout << "[DBG]: Sorted ui elements by index: ";
    for (auto& i : m_UI) 
    {
        std::cout << i->Index << ' ';
    }
    std::cout << std::endl;
#endif // _DEBUG
}

void HX3::Output::LoopPrint() 
{
    while(m_IsWorking.load()) 
    {
        usleep(16000);

        m_UpdateLock.lock();
#ifndef _DEBUG
        this->Print();
#endif // _DEBUG
        m_UpdateLock.unlock();
    }
}

// ---------------------------------------------------------------------------- 
void HX3::Output::Update() 
{
    struct winsize newSize;                 

    // Check for updates
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &newSize);    
    if (m_uCurWindowDim.x == newSize.ws_col &&
        m_uCurWindowDim.y == newSize.ws_row) 
    {
        // Nothing to do, return
        return;
    }
    
    m_UpdateLock.lock();

    m_uCurWindowDim.x = newSize.ws_col;
    m_uCurWindowDim.y = newSize.ws_row;
    newSize.ws_col -= 1;

    HX_DBG_PRT_B("New terminal dimensions: columns: ");
    HX_DBG_PRT_I(newSize.ws_col);
    HX_DBG_PRT_I(" rows: ");
    HX_DBG_PRT_I(newSize.ws_row);
    HX_DBG_PRT_E("");
    
    // Update the elements
    for (auto& i : this->m_UI) 
    {
        if (i->Dimensions.UsePercentage) {
           
            i->Dimensions.CalculatedValues.Top =
               i->Dimensions.Defaults.Percent.Top * newSize.ws_row;

            i->Dimensions.CalculatedValues.Bottom =
               i->Dimensions.Defaults.Percent.Bottom * newSize.ws_row;

            i->Dimensions.CalculatedValues.Left =
               i->Dimensions.Defaults.Percent.Left * newSize.ws_col;

            i->Dimensions.CalculatedValues.Right =
               i->Dimensions.Defaults.Percent.Right * newSize.ws_col;
        }
        // Elements that aren't based on percents need to be initialized
        else if (!i->Dimensions.UsePercentage &&
                 (!i->Dimensions.CalculatedValues.Top ||
                  !i->Dimensions.CalculatedValues.Right ||
                  !i->Dimensions.CalculatedValues.Left ||
                  !i->Dimensions.CalculatedValues.Bottom)) {

            i->Dimensions.CalculatedValues.Top    = i->Dimensions.Defaults.Cells.Top;
            i->Dimensions.CalculatedValues.Bottom = i->Dimensions.Defaults.Cells.Bottom;
            i->Dimensions.CalculatedValues.Left   = i->Dimensions.Defaults.Cells.Left;
            i->Dimensions.CalculatedValues.Right  = i->Dimensions.Defaults.Cells.Right;
        }

        HX_DBG_PRT_B("Element index: ");
        HX_DBG_PRT_I(i->Index);
        HX_DBG_PRT_I(" top: ");
        HX_DBG_PRT_I(i->Dimensions.CalculatedValues.Top);
        HX_DBG_PRT_I(" bottom: ");
        HX_DBG_PRT_I(i->Dimensions.CalculatedValues.Bottom);
        HX_DBG_PRT_I(" left: ");
        HX_DBG_PRT_I(i->Dimensions.CalculatedValues.Left);
        HX_DBG_PRT_I(" right: ");
        HX_DBG_PRT_I(i->Dimensions.CalculatedValues.Right);
        HX_DBG_PRT_E("");
    }

    m_UpdateLock.unlock();
}

// ---------------------------------------------------------------------------- 
void HX3::Output::ClearBackBuffer() 
{
    for (uint64_t i = 0; i < m_uCurWindowDim.y; ++i) 
    {
        for (uint64_t k = 0; k < m_uCurWindowDim.x; ++k) 
        {
            m_Buffers.Back[i][k] = ' ';
        }
    }
}

// ---------------------------------------------------------------------------- 
void HX3::Output::Print() 
{
    this->ClearBackBuffer();
    
    int32_t k, j;
    int32_t width, height;
    for (size_t i = 0; i < m_UI.size(); ++i) 
    {
        auto& item = m_UI[i];
        auto& dim = item->Dimensions;

        // Call reset to indicate that we are printing the element once again
        item->Content->Reset();

        width = 
            dim.CalculatedValues.Right - dim.CalculatedValues.Left;
        height = 
            dim.CalculatedValues.Bottom - dim.CalculatedValues.Top;

        // Recalculate if element should be always visible and isn't
        if ((width <= 0) &&
            item->Flags & UI::UiFlags::AlwaysVisible) 
        {
            if (dim.CalculatedValues.Left != static_cast<int32_t>(m_uCurWindowDim.x - 1)) 
            {
                dim.CalculatedValues.Right = std::move(
                    dim.CalculatedValues.Left + 1);
            }
            else 
            {
                dim.CalculatedValues.Left = std::move(
                        dim.CalculatedValues.Left - 1);
            }
        }

        if ((height <= 0) &&
            item->Flags & UI::UiFlags::AlwaysVisible)
        {

            if (dim.CalculatedValues.Top != static_cast<int32_t>(m_uCurWindowDim.y)) 
            {
                dim.CalculatedValues.Bottom = std::move(
                        dim.CalculatedValues.Top + 1);
            }
            else 
            {
                dim.CalculatedValues.Top = std::move(
                        dim.CalculatedValues.Top - 2);
            }

        }
        
        for (k = dim.CalculatedValues.Top;
             (k < dim.CalculatedValues.Bottom && 
              k < static_cast<int>(m_uCurWindowDim.y));
             ++k) 
        {
            for (j = dim.CalculatedValues.Left; 
                (j < dim.CalculatedValues.Right && 
                 j < static_cast<int>(m_uCurWindowDim.x));
                 ++j)
            {

                 m_Buffers.Back[k][j] = std::move(item->Content->GetData(j, k));
            }
        }
    }

    for (uint64_t i = 0; i < (m_uCurWindowDim.y - 1); ++i) 
    {
        m_Buffers.Back[i][m_uCurWindowDim.x - 1] = '\n';
        m_Buffers.Back[i][m_uCurWindowDim.x] = '\0';
        std::cout << m_Buffers.Back[i];
    }

    m_Buffers.Swap();
}
