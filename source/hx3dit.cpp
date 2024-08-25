#include "hx3dit.hpp"
#include "editor.hpp"
#include "input.hpp"
#include "output.hpp"

int main(int argc, char* argv[]) 
{
    using namespace std;
    using namespace HX3;
    using fs = std::fstream;

#ifdef _DEBUG
    // Load debug arguments
    if (argc < 2) 
    {
        HX_DBG_PRT_N("Loading debug arguments");

        argc = 2;

        char** debug_argv = new char*[2];
        debug_argv[0] = argv[0];
       
        // Construct a path to the test_file
        constexpr int test_file_path_size = 256;
        char* test_file_path = new char[test_file_path_size];

        if (strlen(argv[0]) >= test_file_path_size) {
            throw std::runtime_error("Cannot copy argv[0] to test_file_path, test_file_path is too samll");
        }
        strcpy(test_file_path, argv[0]);
       
        const char* to_test_file_from_bin = "/../test_file";
        char* last_slash = strrchr(test_file_path, '\\');
        if (!last_slash) last_slash = strrchr(test_file_path, '/');
        if (last_slash) {
            strcpy(last_slash, to_test_file_from_bin);
        }
        
        debug_argv[1] = test_file_path;
        argv = debug_argv;
    }

    HX_DBG_PRT_N("Called with following arguments:");
    HX_DBG_PRT_FE(argv, argc);
#endif // _DEBUG

    // Decide if passed arguments are viable
    if (argc < 2 ||
        // strcmp() returns 0 if strings are identical
        !strcmp(argv[1], "--help") ||
        !strcmp(argv[1], "-h") ||
        !fs(argv[1]).is_open()) 
    {
        cout << "Passed argument is invalid." << endl
             << endl
             << "Use: " << endl
             << "   hx3ditr [Path to a file]" << endl;

        return -1;
    }
    
    // Initialize variables;
    shared_ptr<Editor> myEditor = make_shared<Editor>();
    myEditor->SetFile(argv[1]);
    
    // Predefined UI elements
    static HX3::Byte verticalSpacer = '|';
    static HX3::Byte horizontalSpacer = '-';
    static HX3::Byte decMode[] = "Dec";
    static HX3::Byte hexMode[] = "Hex";
    static HX3::Byte octMode[] = "Oct";
    static HX3::Byte modeText[] = "Mode:";
    static HX3::Byte interpretationText[] = "Interpretation:";

    // Describe all of UI elements
    shared_ptr<UI::UiDataViewer> myEditorDataViewer = make_shared<UI::UiDataViewer>();
    myEditorDataViewer->Index = 10;
    myEditorDataViewer->Dimensions.Defaults.Cells = { 6, 512, 1, 54 };

    shared_ptr<UI::UiSpacer> mySpacer1 = make_shared<UI::UiSpacer>();
    mySpacer1->Index = 997;
    mySpacer1->Dimensions.UsePercentage = true;
    mySpacer1->Flags = UI::UiFlags::AlwaysVisible;
    mySpacer1->Dimensions.Defaults.Percent = { 0.f, 1.f, 0.f, 0.f };

    shared_ptr<UI::UiSpacer> mySpacer2 = make_shared<UI::UiSpacer>();
    mySpacer2->Index = 997;
    mySpacer2->Dimensions.UsePercentage = true;
    mySpacer2->Flags = UI::UiFlags::AlwaysVisible;
    mySpacer2->Dimensions.Defaults.Percent = { 1.f, 1.f, 0.f, 1.f };

    shared_ptr<UI::UiSpacer> mySpacer3 = make_shared<UI::UiSpacer>();
    mySpacer3->Index = 998;
    mySpacer3->Dimensions.UsePercentage = true;
    mySpacer3->Flags = UI::UiFlags::AlwaysVisible;
    mySpacer3->Dimensions.Defaults.Percent = { 0.f, 0.f, 0.f, 1.f };

    shared_ptr<UI::UiSpacer> mySpacer4 = make_shared<UI::UiSpacer>();
    mySpacer4->Index = 998;
    mySpacer4->Dimensions.UsePercentage = true;
    mySpacer4->Flags = UI::UiFlags::AlwaysVisible;
    mySpacer4->Dimensions.Defaults.Percent = { 0.f, 1.f, 1.f, 1.f };

    shared_ptr<UI::UiSpacer> mySpacer5 = make_shared<UI::UiSpacer>();
    mySpacer5->Index = 997;
    mySpacer5->Dimensions.Defaults.Cells = { 53, 512, 0, 54 }; 

    shared_ptr<UI::UiSpacer> mySpacer6 = make_shared<UI::UiSpacer>();
    mySpacer6->Index = 997;
    mySpacer6->Dimensions.Defaults.Cells = { 5, 512, 0, 6 };

    shared_ptr<UI::UiText> myModeSwitch = make_shared<UI::UiText>();
    myModeSwitch->Index = 500;
    myModeSwitch->Dimensions.Defaults.Cells = { 57, 4, 3, 57 + sizeof(decMode) - 1 };

    shared_ptr<UI::UiText> myModeText = make_shared<UI::UiText>();
    myModeText->Index = 500;
    myModeText->Dimensions.Defaults.Cells = { 57, 3, 2, 57 + sizeof(modeText) - 1 };

    shared_ptr<UI::UiText> myInterpretationText = make_shared<UI::UiText>();
    myInterpretationText->Index = 500;
    myInterpretationText->Dimensions.Defaults.Cells = { 57, 6, 5, 57 + sizeof(interpretationText) - 1 };
    
    // Bind data to UI elements
    myEditorDataViewer->SetData(
            myEditor->GetPtrToCurPos(), 
            myEditor->GetFileSize());
    mySpacer1->SetData(&verticalSpacer, 0);
    mySpacer2->SetData(&verticalSpacer, 0);
    mySpacer3->SetData(&horizontalSpacer, 0);
    mySpacer4->SetData(&horizontalSpacer, 0);
    mySpacer5->SetData(&verticalSpacer, 0);
    mySpacer6->SetData(&verticalSpacer, 0);
    myModeSwitch->SetData(decMode, sizeof(decMode));
    myModeText->SetData(modeText, sizeof(modeText));
    myInterpretationText->SetData(interpretationText, sizeof(interpretationText));
    
    // Add elements to output queue
    shared_ptr<Output> myOutput = make_shared<Output>();
    myOutput->AddElement(myEditorDataViewer);
    myOutput->AddElement(mySpacer1);
    myOutput->AddElement(mySpacer2);
    myOutput->AddElement(mySpacer3);
    myOutput->AddElement(mySpacer4);
    myOutput->AddElement(mySpacer5);
    myOutput->AddElement(mySpacer6);
    myOutput->AddElement(myModeSwitch);
    myOutput->AddElement(myModeText);
    myOutput->AddElement(myInterpretationText);

    // Set up the editor
    shared_ptr<Input> myInput = make_shared<Input>();
    shared_ptr<UserControls> myUserContorls = make_shared<UserControls>();
 
    myUserContorls->SetDataView(myEditorDataViewer);
    myUserContorls->SetEditor(myEditor);
    myInput->SetUserControls(myUserContorls);

    myInput->Initialize();
    myOutput->Initialize();

    while (myEditor->GetStatus()) 
    {
        // Update main data view
        reinterpret_cast<UI::DataViewer*>(
                myEditorDataViewer->Content.get())->UpdateData(
                myEditor->GetPtrToCurPos(),
                myEditor->GetFileSize() - myEditor->GetCurPos());
    
        // Update mode switch 
        auto& mode = reinterpret_cast<UI::DataViewer*>(
                myEditorDataViewer->Content.get())->GetCurrentMode();

        if (mode == UI::Dec) 
            myModeSwitch->SetData(decMode, sizeof(decMode));
        else if (mode == UI::Hex) 
            myModeSwitch->SetData(hexMode, sizeof(hexMode));
        else if (mode == UI::Oct) 
            myModeSwitch->SetData(octMode, sizeof(octMode));

        // Update scale
        myOutput->Update();
    }
    
    return 0;
}
