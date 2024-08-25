#include "hx3dit.hpp"

#include "editor.hpp"

using namespace std;

// ---------------------------------------------------------------------------- 
HX3::Editor::~Editor()
{
    this->ReleaseFileData();
}

// ---------------------------------------------------------------------------- 
void HX3::Editor::SetFile(const char* szPath) 
{
    using byte = HX3::Byte;
    using ifs = std::ifstream;
    using std::filesystem::file_size;

    // ReleaseFileData() checks for nullptrs, so we don't have to do it here
    this->ReleaseFileData();

    ifs file = ifs(szPath, std::ios::binary | std::ios_base::in);
    if (!file.is_open())
        throw std::invalid_argument("szPath");
    
    m_uFileSize = file_size(szPath);

    m_pRawLoadedFile = reinterpret_cast<byte*>(
            malloc(sizeof(::byte) * m_uFileSize));

    file.read(reinterpret_cast<char*>(m_pRawLoadedFile), m_uFileSize);

    file.close();
}

// ---------------------------------------------------------------------------- 
HX3::Byte* HX3::Editor::GetPtrToPos(const uint64_t& uPos) 
{
    if (!m_pRawLoadedFile) throw;

    if (uPos >= m_uFileSize) return &m_pRawLoadedFile[m_uFileSize];

    return &m_pRawLoadedFile[uPos];
}

// ---------------------------------------------------------------------------- 
void HX3::Editor::MovePos(const int64_t& distance) 
{
    if (!m_pRawLoadedFile) throw;

    uint64_t newAmount = m_uFileCurrentPos + distance;
  
    // Protect from setting position outside of buffer with negative
    // and positive distance
    if (distance < 0 &&
        static_cast<uint64_t>(-1 * distance) > m_uFileCurrentPos)
    {
        newAmount = 0;
    }
    if (newAmount >= m_uFileSize)
    {
        newAmount = m_uFileSize - 1;
    }
   
    m_uFileCurrentPos = std::move(newAmount);

    HX_DBG_PRT_B("Current position in file: [");
    HX_DBG_PRT_I(m_uFileCurrentPos);
    HX_DBG_PRT_I("/");
    HX_DBG_PRT_I(m_uFileSize - 1);
    HX_DBG_PRT_I("] Current symbol: [");
    HX_DBG_PRT_I((int)*(this->GetPtrToCurPos()));
    HX_DBG_PRT_E("]");
}

// ---------------------------------------------------------------------------- 
void HX3::Editor::ReleaseFileData() 
{
    if (m_pRawLoadedFile) 
    {
        delete[] m_pRawLoadedFile;
        m_pRawLoadedFile = nullptr;
    }
}
