#ifndef HX_INPUT_H
#define HX_INPUT_H

#include "hx3dit.hpp"
#include "editor.hpp"
#include "output.hpp"

namespace HX3 
{
// ---------------------------------------------------------------------------- 
    enum BindableActions 
    {
        MoveLeft,  
        MoveDown,
        MoveUp,  
        MoveRight,  
        Exit,
        Increment,
        Decrement,
        SwitchDisplayMode
    };
    
// ---------------------------------------------------------------------------- 
    typedef std::unordered_map<
        HX3::KeyboardKey, HX3::BindableActions> Keybinds;

// ---------------------------------------------------------------------------- 
    class UserControls 
    {
        uint32_t m_uActionMul = 0;
        std::shared_ptr<HX3::Editor> m_pEditor = nullptr;
        std::shared_ptr<HX3::UI::UiElement> m_pDataView = nullptr;

    public:
        UserControls() = default;
        UserControls(UserControls &&) = default;
        UserControls(const UserControls &) = default;
        UserControls &operator=(UserControls &&) = default;
        UserControls &operator=(const UserControls &) = default;
        ~UserControls() = default;

    public:
        void SetActionMul(const char& mul);

        void SetEditor(std::shared_ptr<HX3::Editor> editor);

        void SetDataView(std::shared_ptr<HX3::UI::UiElement> dataView);

    public:
        void DoAction(const BindableActions& act);

    };

// ---------------------------------------------------------------------------- 
    /**
     * Manages input. 
     * Loads key binds, catches key presses, dispatches them to UserControls
     **/
// ---------------------------------------------------------------------------- 
    class Input 
    {
        std::thread* m_pThread = nullptr;
        std::atomic_bool m_IsWorking = false;

        std::shared_ptr<HX3::Keybinds> m_pKeybinds = nullptr;
        std::shared_ptr<HX3::UserControls> m_pControls = nullptr;

    public:
        Input() = default;
        Input(Input &&) = delete;
        Input(const Input &) = delete;
        Input &operator=(Input &&) = delete;
        Input &operator=(const Input &) = delete;
        ~Input();

    public:
        void SetKeybinds(std::shared_ptr<HX3::Keybinds> keybinds);

        void SetUserControls(std::shared_ptr<HX3::UserControls> userControls);
    
    public:
        void Initialize();

        void Destroy();

    private:
        void CatchKeys();

        std::shared_ptr<HX3::Keybinds> GetDefaultKeybinds() 
        {
            std::shared_ptr<HX3::Keybinds> defaults = 
                std::make_shared<HX3::Keybinds>();

            defaults->insert_or_assign('q', HX3::BindableActions::Exit);

            defaults->insert_or_assign('h', HX3::BindableActions::MoveLeft);
            defaults->insert_or_assign('j', HX3::BindableActions::MoveDown);
            defaults->insert_or_assign('k', HX3::BindableActions::MoveUp);
            defaults->insert_or_assign('l', HX3::BindableActions::MoveRight);

            defaults->insert_or_assign('a', HX3::BindableActions::Increment);
            defaults->insert_or_assign('x', HX3::BindableActions::Decrement);
            
            defaults->insert_or_assign('m', HX3::BindableActions::SwitchDisplayMode);

            return defaults;
        }
    };
}

#endif // !HX_INPUT_H
