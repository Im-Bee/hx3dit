#include "hx3dit.hpp"
#include "editor.hpp"
#include "output.hpp"

#include "input.hpp"

using namespace std;
using namespace HX3;

void HX3::UserControls::SetActionMul(const char& mul) 
{
    // Move multiplier to the left to make space for next digit
    if (m_uActionMul) m_uActionMul *= 10;

    m_uActionMul += mul - AsciiZero;

    HX_DBG_PRT_B("Action multiplier set to ");
    HX_DBG_PRT_I(m_uActionMul);
    HX_DBG_PRT_E("");
}


void HX3::UserControls::SetEditor(std::shared_ptr<HX3::Editor> editor) 
{
    m_pEditor = editor;
}

void HX3::UserControls::SetDataView(std::shared_ptr<HX3::UI::UiElement> dataView) 
{
    m_pDataView = dataView;
}

void HX3::UserControls::DoAction(const HX3::BindableActions& act) 
{
    if (!m_pEditor.get()) 
    {
        HX_DBG_PRT_N("UserControls doesn't have any Editor");
        return;
    }

    int64_t calc, distance;
    HX3::UI::DataViewer* myDataViewer = nullptr;
    switch (act) 
    {
        case BindableActions::Exit:
            HX_DBG_PRT_N("Exit");
            m_pEditor->SetStatusExit();
            break;

        case BindableActions::MoveLeft: 
            HX_DBG_PRT_N("MoveLeft");
            m_pEditor->MovePos(-1);
            break;
        
        case BindableActions::MoveRight: 
            HX_DBG_PRT_N("MoveRight");
            m_pEditor->MovePos(1);
            break;

        case BindableActions::MoveUp: 
            HX_DBG_PRT_N("MoveUp");
            calc =
                m_pDataView->Dimensions.CalculatedValues.Right - 
                m_pDataView->Dimensions.CalculatedValues.Left;
            distance = 0;
            while (calc > 3) {
                calc -= 4;
                ++distance;
            }
            m_pEditor->MovePos(-1 * distance);
            break;

        case BindableActions::MoveDown: 
            HX_DBG_PRT_N("MoveDown");
            calc = 
                m_pDataView->Dimensions.CalculatedValues.Right - 
                m_pDataView->Dimensions.CalculatedValues.Left;
            distance = 0;
            while (calc > 3) {
                calc -= 4;
                ++distance;
            }
            m_pEditor->MovePos(distance);
            break;

        case BindableActions::Increment:
            HX_DBG_PRT_N("Increment");
            ++(*m_pEditor->GetPtrToCurPos());
            break;

        case BindableActions::Decrement:
            HX_DBG_PRT_N("Decrement");
            --(*m_pEditor->GetPtrToCurPos());
            break;

        case BindableActions::SwitchDisplayMode:
            HX_DBG_PRT_N("DisplayMode");
            myDataViewer = reinterpret_cast<HX3::UI::DataViewer*>(
                    this->m_pDataView->Content.get());
            calc = myDataViewer->GetCurrentMode();
            if (++calc >= HX3::UI::DataRepresentationSize)
                calc = 0;

            if (calc == 0)
                myDataViewer->SetMode(UI::Hex);
            else if (calc == 1)
                myDataViewer->SetMode(UI::Oct);
            else if (calc == 2)
                myDataViewer->SetMode(UI::Dec);
            break;

        default:
            HX_DBG_PRT_N("Controls got unknown action?");
    }

    if (m_uActionMul) 
    {
        // Copy the multiplier to avoid infinite loop
        uint32_t cpy = m_uActionMul;
        m_uActionMul = 0;

        for (uint32_t i = 1; i < cpy; ++i) 
            this->DoAction(act);
    }
}

HX3::Input::~Input() 
{
    this->Destroy();
}

void HX3::Input::Initialize() 
{
    HX_DBG_PRT_N("Initializing input thread");

    if (m_IsWorking.load() || m_pThread) 
        throw runtime_error("Cannot initialize the same input second time");

    m_IsWorking = true;
    m_pThread = new thread(&HX3::Input::CatchKeys, this);
}

void HX3::Input::Destroy() 
{
    HX_DBG_PRT_N("Killing input thread");

    if (m_pThread) 
    {
        m_IsWorking.store(false);
        m_pThread->join();
        m_pThread = nullptr;
    }
}

void HX3::Input::SetUserControls(std::shared_ptr<HX3::UserControls> userControls) 
{
    if (m_IsWorking.load()) {
        HX_DBG_PRT_N("Can't link editor controls to input if input is working");
        return;
    }

    m_pControls = userControls;
}

void HX3::Input::CatchKeys() 
{
    struct termios oldSettings, newSettings;

    if (!m_pKeybinds.get())
        m_pKeybinds = this->GetDefaultKeybinds();
    
    tcgetattr(fileno(stdin), &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &newSettings);

    char c;
    while (m_IsWorking.load()) 
    {
        if (!m_pControls)
            continue;
     
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fileno(stdin), &set);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        int res = select(
                fileno(stdin) + 1, 
                &set, 
                NULL, 
                NULL,
                &tv);

        if (res > 0) 
        {
            read(fileno(stdin), &c, 1);
        }
        else 
        {
            HX_DBG_PRT_N("Result on input is <= 0");
            continue;
        }
        
        // Check for numbers
        if (c >= AsciiZero && c <= AsciiNine) 
        {
            m_pControls->SetActionMul(c);
            continue;
        }
       
        // Check for actions
        auto action = m_pKeybinds->find(c);
        if (action == m_pKeybinds->end()) 
        {
            continue;
        }
        m_pControls->DoAction(action->second);
    }

    tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
}

