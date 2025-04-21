#include "Editor.h"

using namespace rapidjson;


int Editor::EditorBegin()
{

    SDL_WindowFlags window_Flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI); // Allows window to be reajusted and run on high def monitors 
    window = SDL_CreateWindow("IonEngine  Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_Flags); // Create window in center of screen with 1280x720 dimensions 
    
    // Set up dear ImGui context 
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    // set up the style
    ImGui::StyleColorsDark(); 

    // Set up thebackends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    
    // check to see if there is a resource folder
    if (check_for_resourec_folder())
    {
        return 1;
    }
    else
    {
        // At this point, I garuntee that there is no config file, so now we can make one
        create_config_file();
        create_render_config_file();
    }

    editor();



// The windowhas been requested to be closed. Output all files
create_scene();

// Cleanup
ImGui_ImplSDLRenderer2_Shutdown();
ImGui_ImplSDL2_Shutdown();
ImGui::DestroyContext();

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();

return 0;
}




bool Editor::check_for_resourec_folder()
{
    if (std::filesystem::is_directory("resources"))
    {
        // Cleanup
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return true;
    }
    else
    {
        std::filesystem::create_directory("resources");
        return false;
    }
}

bool Editor::check_for_config_file()
{
    if(std::filesystem::exists("resources/game.config"))
    {
        return true;
    }
    return false;
}

bool Editor::check_for_rendering_config()
{
    if (std::filesystem::exists("resources/rendering.config"))
    {
        return true;
    }
    return false;
}






// Requires: current window, current renderer, the Gui Io, and the clear color for SDL. 
// 
void Editor::create_config_file()
{

    bool done = false;
    std::vector<std::string> configureKeyWords{ "game_title", "initial_scene" };
    std::vector<std::array<char, 32>> configureData = { {}, {} };
    int index = 0;
    bool titleConfigure = false;
    bool initialSceneConfigured = false;
    ImVec2 position = ImVec2(500, 300);
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }
        // ALWAYS COMES BEFORE ANY GUI COMMANDS
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // ALWAYS COMES BEFORE ANY GUI COMMANDS


        //ImGui::Text(configDataArray[index].c_str());
        ImGui::SetNextWindowPos(ImVec2(position.x, position.y), ImGuiCond_Always);
        if (!titleConfigure || !initialSceneConfigured)
        {
            ImGui::OpenPopup("Please Configure the game");
            if (ImGui::BeginPopup("Please Configure the game"))
            {
                ImGui::Text("Game Title: ");
                ImGui::SameLine();
                if (ImGui::InputText("##titleText", configureData[0].data(), configureData[0].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    titleConfigure = true;
                }
                ImGui::Text("Init Scene: ");
                ImGui::SameLine();
                if (ImGui::InputText("##InitSceneText", configureData[1].data(), configureData[1].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    currentSceneName = configureData[1].data();
                    initialSceneConfigured = true;
                }
                ImGui::EndPopup();
            }
        }

        if (titleConfigure && initialSceneConfigured)
        {
            // You have entered a title for the game as a well as an initial scene name
            done = true;
            basic_print_JSON_to_file("resources/game.config", configureKeyWords, configureData);
        }

        ImGui::Render();
        SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

void Editor::create_render_config_file()
{
    bool done = false;
    std::vector<std::string> renderConfigKeyWords{ "clear_color_r", "clear_color_g", "clear_color_b", "x_resolution", "y_resolution"};
    int index = 0;
    std::vector<std::array<char, 32>> renderConfigData = { {}, {}, {}, {}, {} };
    ImVec2 position = ImVec2(500, 300);
   

    bool red = false;
    bool green = false;
    bool blue = false;
    bool x_res = false;
    bool y_res = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }
        // ALWAYS COMES BEFORE ANY GUI COMMANDS
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // ALWAYS COMES BEFORE ANY GUI COMMANDS

        ImGui::SetNextWindowPos(ImVec2(position.x, position.y), ImGuiCond_Always);
        if (!red || !green || !blue || !x_res || !y_res)
        {
            ImGui::OpenPopup("Please configure rendering");
            if (ImGui::BeginPopup("Please configure rendering"))
            {
                ImGui::Text("Clear Color Red:   ");
                ImGui::SameLine();
                if (ImGui::InputText("##ColorRed", renderConfigData[0].data(), renderConfigData[0].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    red = true;
                }
                ImGui::Text("Clear Color Green: ");
                ImGui::SameLine();
                if (ImGui::InputText("##ColorGreen", renderConfigData[1].data(), renderConfigData[1].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    green = true;
                }
                ImGui::Text("Clear Color Blue:  ");
                ImGui::SameLine();
                if (ImGui::InputText("##ColorBlue", renderConfigData[2].data(), renderConfigData[2].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    blue = true;
                }
                ImGui::Text("X Resolution:      ");
                ImGui::SameLine();
                if (ImGui::InputText("##XResolution", renderConfigData[3].data(), renderConfigData[3].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    x_res = true;
                }
                ImGui::Text("Y Resolution:      ");
                ImGui::SameLine();
                if (ImGui::InputText("##YResolution", renderConfigData[4].data(), renderConfigData[4].size(), ImGuiInputTextFlags_EnterReturnsTrue))  // "##" hides the label
                {
                    y_res = true;
                }
                ImGui::EndPopup();
            }
        }

        if (red && green && blue && x_res && y_res)
        {
            basic_print_JSON_to_file_numbers("resources/rendering.config", renderConfigKeyWords, renderConfigData);
            done = true;
        }

        ImGui::Render();
        SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

// Requires: file name including the path to where the file should be stored, the keywords vector, and the data to be printed
// Effects: Creates a new file in JSON with the given data
// Modifies: 
void Editor::basic_print_JSON_to_file(std::string filename, std::vector<std::string> keywords, std::vector<std::array<char, 32>> data)
{
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        outFile << "{\n";

        for (auto i = 0; i < keywords.size(); i++)
        {
            if (i == keywords.size() - 1)
            {
                // Don't output the comma at the end
                outFile << "    \"" << keywords[i] << "\": \"" << data[i].data() << "\"\n";
            }
            else
            {
                // Output the comma at the end
                outFile << "    \"" << keywords[i] << "\": \"" << data[i].data() << "\",\n";
            }
        }
        outFile << "}";
        outFile.close();
    }
}
void Editor::basic_print_JSON_to_file_numbers(std::string filename, std::vector<std::string> keywords, std::vector<std::array<char, 32>> data)
{
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        outFile << "{\n";

        for (auto i = 0; i < keywords.size(); i++)
        {
            if (i == keywords.size() - 1)
            {
                // Don't output the comma at the end
                outFile << "    \"" << keywords[i] << "\": " << data[i].data() << "\n";
            }
            else
            {
                // Output the comma at the end
                outFile << "    \"" << keywords[i] << "\": " << data[i].data() << ",\n";
            }
        }
        outFile << "}";
        outFile.close();
    }
}

void Editor::editor()
{
    bool done = false;
    std::filesystem::create_directory("resources/images");
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }
        // ALWAYS COMES BEFORE ANY GUI COMMANDS
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // ALWAYS COMES BEFORE ANY GUI COMMANDS

        //---Insert my code---//
        create_actor_list_in_scene();
        add_end_button(&done);
        Render_Editor_Actors();


        //---End my code---//
        
       


        // Always call at the end of each frame
        SDL_SetRenderDrawColor(renderer,
            (Uint8)(clear_color.x * 255),
            (Uint8)(clear_color.y * 255),
            (Uint8)(clear_color.z * 255),
            (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui::Render();
        SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
        render_and_clear_image_queue();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

void Editor::create_actor_list_in_scene()
{
    // Set window size 
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);         // Top-left corner
    ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);


    static int selectedIndex = -1;
    ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    /*static int selectedItem = 0;
    const char* items[] = { "Item A", "Item B", "Item C", "Item D", "Item E" };

    ImGui::ListBox("##ItemList", &selectedItem, items, IM_ARRAYSIZE(items));

    ImGui::End();*/
    
    // Make sure that the map exists so I don't try to index into null memory
    if (currentSceneActorMap.find(currentSceneName) == currentSceneActorMap.end())
    {
        std::shared_ptr<editorActor> temptr(new editorActor);
        temptr->name = "dummy";
        currentSceneActorMap[currentSceneName].push_back(temptr);
    }
    // Set up the scrollable list
    ImGui::BeginChild("Actor List", ImVec2(0, 300), true);
    // add stuff to the scrollable list
    for (size_t i = 0; i < currentSceneActorMap.at(currentSceneName).size(); i++)
    {
        const bool isSelected = (i == selectedIndex);
        if (ImGui::Selectable(currentSceneActorMap.at(currentSceneName)[i]->name.c_str(), isSelected))
        {
            selectedIndex = i;
        }
    }

    ImGui::EndChild();

    // Need to have someway to add an actor
    static bool addActor = false;
    ImGui::Checkbox("Add Actor", &addActor);
    if (addActor)
    {
        char actorName[32];
        ImGui::SetNextItemWidth(50.0f);
        if (ImGui::InputText("Actor Name ", actorName, IM_ARRAYSIZE(actorName), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::shared_ptr<editorActor> temptr(new editorActor);
            temptr->name = actorName;
            currentSceneActorMap[currentSceneName].push_back(temptr);
        }
    }

    ImGui::End();

    if (selectedIndex >= 0)
    {
        Open_Properties_Menu(selectedIndex);
    }
}

void Editor::Open_Properties_Menu(int selected_actor_index)
{
    // Set window size 
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, 0), ImGuiCond_Always);         // Top-left corner
    ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

    // Extract the actor we want
    std::shared_ptr<editorActor> pointer_to_selected_actor(currentSceneActorMap.at(currentSceneName)[selected_actor_index]);

    // Open an actors property window on he right side of the screen
    ImGui::Begin("Actor Properties Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    // Create character data we need to store stuff
    char storingData[32];

    // Output some basic stuff
    // Get name
    ImGui::Text("Name     ");
    ImGui::SameLine();
    strncpy_s(storingData, pointer_to_selected_actor->name.c_str(), sizeof(storingData));
    ImGui::InputText("##CurrentActorName", storingData, IM_ARRAYSIZE(storingData));
    pointer_to_selected_actor->name = storingData;

    // Get transform Data
    ImGui::Text("Transform");
    ImGui::SetNextItemWidth(50.0f);
    ImGui::SameLine(100.0f);
    ImGui::InputFloat("##CurrentActorX", &pointer_to_selected_actor->position.x);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::SameLine(150.0f);
    ImGui::InputFloat("##CurrentActorY", &pointer_to_selected_actor->position.y);
    

    // Rendering Information
    ImGui::Checkbox("Rendering Enable", &pointer_to_selected_actor->isRendering);
    if (pointer_to_selected_actor->isRendering)
    {
        ImGui::Text("Renderer ");
        strncpy_s(storingData, pointer_to_selected_actor->textureName.c_str(), sizeof(storingData));
        if (ImGui::InputText("##CurrentTexture", storingData, IM_ARRAYSIZE(storingData), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            pointer_to_selected_actor->textureName = storingData;
        }

        ImGui::Text("Rotation ");
        ImGui::SameLine();
        ImGui::InputFloat("##rotation", &pointer_to_selected_actor->rotation);

        ImGui::Text("X Scale  ");
        ImGui::SameLine();
        ImGui::InputFloat("##x_scale", &pointer_to_selected_actor->scaleX);

        ImGui::Text("Y Scale  ");
        ImGui::SameLine();
        ImGui::InputFloat("##y_scale", &pointer_to_selected_actor->scaleY);

        ImGui::Text("X Pivot  ");
        ImGui::SameLine();
        ImGui::InputFloat("##x_pivot", &pointer_to_selected_actor->pivotX);

        ImGui::Text("Y Pivot  ");
        ImGui::SameLine();
        ImGui::InputFloat("##y_pivot", &pointer_to_selected_actor->pivotY);

        ImGui::Text("r  ");
        ImGui::SameLine();
        ImGui::InputFloat("##r", &pointer_to_selected_actor->r);

        ImGui::Text("g  ");
        ImGui::SameLine();
        ImGui::InputFloat("##g", &pointer_to_selected_actor->g);

        ImGui::Text("b  ");
        ImGui::SameLine();
        ImGui::InputFloat("##b", &pointer_to_selected_actor->b);

        ImGui::Text("a  ");
        ImGui::SameLine();
        ImGui::InputFloat("##a", &pointer_to_selected_actor->a);

        ImGui::Text("sortOrder");
        ImGui::SameLine();
        ImGui::InputInt("##sortOrder", &pointer_to_selected_actor->sorting_order);
    }
    
    // Component Handling logic
    bool tryingtoAdd = false;
    if (pointer_to_selected_actor->components.size() > 0)
    {
        ImGui::Text("Components:");
    }
    for (auto i = pointer_to_selected_actor->components.begin(); i != pointer_to_selected_actor->components.end(); i++)
    {
        if (i->first != "Rigidbody" && i->first != "ParticleSystem")
        {
            if (ImGui::Button(i->first.c_str()))
            {
                std::string temp = "code resources/component_types/" + i->first + ".lua";
                system(temp.c_str());
            }
        }
        else if (i->first == "Rigidbody")
        {
            ImGui::Text(i->first.c_str());
            strncpy_s(storingData, pointer_to_selected_actor->rigidbody.body_type.c_str(), sizeof(storingData));
            if (ImGui::InputText("Body Type", storingData, IM_ARRAYSIZE(storingData), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pointer_to_selected_actor->rigidbody.body_type = storingData;
            }
            ImGui::Checkbox("precise", &pointer_to_selected_actor->rigidbody.precise);
            
            ImGui::InputFloat("Gravity Scale", &pointer_to_selected_actor->rigidbody.gravity_scale);
            ImGui::InputFloat("Density", &pointer_to_selected_actor->rigidbody.density);
            ImGui::InputFloat("Ang Fric", &pointer_to_selected_actor->rigidbody.angularFriction);
            
            ImGui::Checkbox("hasCollider", &pointer_to_selected_actor->rigidbody.hasCollider);
            ImGui::Checkbox("hasTrigger", &pointer_to_selected_actor->rigidbody.has_trigger);

            strncpy_s(storingData, pointer_to_selected_actor->rigidbody.colliderType.c_str(), sizeof(storingData));
            if (ImGui::InputText("Collider Type", storingData, IM_ARRAYSIZE(storingData), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pointer_to_selected_actor->rigidbody.colliderType = storingData;
            }

            ImGui::InputFloat("width", &pointer_to_selected_actor->rigidbody.width);
            ImGui::InputFloat("height", &pointer_to_selected_actor->rigidbody.height);
            ImGui::InputFloat("radius", &pointer_to_selected_actor->rigidbody.radius);
            ImGui::InputFloat("friction", &pointer_to_selected_actor->rigidbody.friction);
            ImGui::InputFloat("bounciness", &pointer_to_selected_actor->rigidbody.bounciness);

            strncpy_s(storingData, pointer_to_selected_actor->rigidbody.triggerType.c_str(), sizeof(storingData));
            if (ImGui::InputText("triggerType", storingData, IM_ARRAYSIZE(storingData), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                pointer_to_selected_actor->rigidbody.triggerType = storingData;
            }
            
            ImGui::InputFloat("triggerWidth", &pointer_to_selected_actor->rigidbody.triggerWidth);
            ImGui::InputFloat("triggerHeight", &pointer_to_selected_actor->rigidbody.triggerHeight);
            ImGui::InputFloat("triggerRadius", &pointer_to_selected_actor->rigidbody.triggerRadius);
        }
        else
        {
            ImGui::Text(i->first.c_str());
        }
    }

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("##ComponentDropDown");
    }
    bool tryingToAdd = false;
    if (ImGui::BeginPopup("##ComponentDropDown"))
    {
        if (ImGui::Selectable("Rigidbody"))
        {
            pointer_to_selected_actor->components["Rigidbody"] = std::to_string(keyCounter);
            keyCounter++;
        }
        if (ImGui::Selectable("ParticleSystem"))
        {
            pointer_to_selected_actor->components["ParticleSystem"] = std::to_string(keyCounter);
            keyCounter++;
        }
        for (auto i = available_components.begin(); i != available_components.end(); i++)
        {
            if (ImGui::Selectable(i->first.c_str()))
            {
                pointer_to_selected_actor->components[i->first.c_str()] = std::to_string(keyCounter);
                keyCounter++;
            }
        }
        if (ImGui::Selectable("Add+"))
        {
            tryingToAdd = true;
        }
        ImGui::EndPopup();
    }

    static char temp[32] = "";
    if (tryingToAdd)
    {
        ImGui::OpenPopup("##AddingComponents");
    }
    if (ImGui::BeginPopup("##AddingComponents"))
    {
        if (ImGui::InputText("New Component Name:", temp, IM_ARRAYSIZE(temp), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            // Create new component
            pointer_to_selected_actor->components[temp] = std::to_string(keyCounter);
            keyCounter++;

            // Check file existence
            if (std::filesystem::exists("resources/component_types"))
            {
                std::string filepath = std::string("resources/component_types/") + temp + ".lua";
                if (!std::filesystem::exists(filepath))
                {
                    // You could create the file here if needed
                    std::ofstream file(filepath);
                    file << temp << " = {\n";
                    file << "\n";
                    file << "   OnStart = function(self)\n";
                    file << "       Debug.Log(\"Hello\")\n";
                    file << "   end,\n\n";
                    file << "   OnUpdate = function(self)\n";
                    file << "       Debug.Log(\"Hello\")\n";
                    file << "   end\n\n";
                    file << "}";
                    file.close();
                }
            }
            else
            {
                std::filesystem::create_directory("resources/component_types");
                std::string filepath = std::string("resources/component_types/") + temp + ".lua";
                if (!std::filesystem::exists(filepath))
                {
                    // You could create the file here if needed
                    std::ofstream file(filepath);
                    file << temp << " = {\n";
                    file << "\n";
                    file << "   OnStart = function(self)\n";
                    file << "       Debug.Log(\"Hello\")\n";
                    file << "   end,\n\n";
                    file << "   OnUpdate = function(self)\n";
                    file << "       Debug.Log(\"Hello\")\n";
                    file << "   end\n\n";
                    file << "}";
                    file.close();
                }
            }

            // Reset and close
            available_components[temp] = "whomp";
            temp[0] = '\0';  // clear input
            ImGui::CloseCurrentPopup();
            tryingToAdd = false;
        }

        ImGui::EndPopup();
    }



    // Custom Feature Logic
    ImGui::Checkbox("Add Custom Feature", &addCustom);

    if (addCustom)
    {
        static char Data[32];
        static char keyword[32];
        static bool keywordReady = false;
        static bool DataReady = false;
        if (ImGui::InputText("Keyword", keyword, IM_ARRAYSIZE(keyword), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            keywordReady = true;
        }
        if (ImGui::InputText("Data", Data, IM_ARRAYSIZE(Data), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            DataReady = true;
        }
        if (keywordReady && DataReady)
        {
            // Need to be pushed in pairs
            pointer_to_selected_actor->custom_keywords.push_back(keyword);
            pointer_to_selected_actor->custom_data.push_back(Data);
            keywordReady = false;
            DataReady = false;
        }
    }

    for (size_t i = 0; i < pointer_to_selected_actor->custom_keywords.size(); i++)
    {
        ImGui::Text(pointer_to_selected_actor->custom_keywords[i].c_str());
        ImGui::SameLine();
        strncpy_s(storingData, pointer_to_selected_actor->custom_data[i].c_str(), sizeof(storingData));
        ImGui::InputText("##CurrentCustomData", storingData, IM_ARRAYSIZE(storingData));
        pointer_to_selected_actor->custom_data[i] = storingData;
    }

    ImGui::End();
}

void Editor::create_scene()
{
    advanced_JSON_output(currentSceneActorMap.at(currentSceneName));
}

void Editor::advanced_JSON_output(std::vector<std::shared_ptr<editorActor>> actors)
{
    std::filesystem::create_directory("resources/scenes");

    // 2. Create scene file
    std::string filename = "resources/scenes/" + currentSceneName + ".scene";
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    // 3. Build JSON
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();

    Value actorsArray(kArrayType);
    for (const auto& actor : actors)
    {
        Value actorObj(kObjectType);
        actorObj.AddMember("name", StringRef(actor->name.c_str()), allocator);
        actorObj.AddMember("x", actor->position.x, allocator);
        actorObj.AddMember("y", actor->position.y, allocator);

        // Add custom keywords
        for (size_t i = 0; i < actor->custom_keywords.size(); ++i)
        {
            actorObj.AddMember(
                StringRef(actor->custom_keywords[i].c_str()),
                StringRef(actor->custom_data[i].c_str()),
                allocator
            );
        }

        // Add components
        if (!actor->components.empty())
        {
            Value componentsObj(kObjectType);
            for (const auto& comp : actor->components)
            {
                Value compObj(kObjectType);
                compObj.AddMember("type", StringRef(comp.first.c_str()), allocator);
                if (comp.first == "Rigidbody")
                {
                    compObj.AddMember("x", actor->position.x, allocator);
                    compObj.AddMember("y", actor->position.y, allocator);
                    compObj.AddMember("scaleX", actor->scaleX, allocator);
                    compObj.AddMember("scaleY", actor->scaleY, allocator);
                    compObj.AddMember("pivotX", actor->pivotX, allocator);
                    compObj.AddMember("pivotY", actor->pivotY, allocator);
                    compObj.AddMember("body_type", StringRef(actor->rigidbody.body_type.c_str()), allocator);
                    compObj.AddMember("precise", actor->rigidbody.precise, allocator);
                    compObj.AddMember("gravity_scale", actor->rigidbody.gravity_scale, allocator);
                    compObj.AddMember("density", actor->rigidbody.density, allocator);
                    compObj.AddMember("angularFriction", actor->rigidbody.angularFriction, allocator);
                    compObj.AddMember("rotation", actor->rotation, allocator);
                    compObj.AddMember("hasCollider", actor->rigidbody.hasCollider, allocator);
                    compObj.AddMember("has_trigger", actor->rigidbody.has_trigger, allocator);
                    compObj.AddMember("colliderType", StringRef(actor->rigidbody.colliderType.c_str()), allocator);
                    compObj.AddMember("width", actor->rigidbody.width, allocator);
                    compObj.AddMember("height", actor->rigidbody.height, allocator);
                    compObj.AddMember("radius", actor->rigidbody.radius, allocator);
                    compObj.AddMember("bounciness", actor->rigidbody.bounciness, allocator);
                    compObj.AddMember("triggerType", StringRef(actor->rigidbody.triggerType.c_str()), allocator);
                    compObj.AddMember("triggerWidth", actor->rigidbody.triggerWidth, allocator);
                    compObj.AddMember("triggerHeight", actor->rigidbody.triggerHeight, allocator);
                    compObj.AddMember("triggerRadius", actor->rigidbody.triggerRadius, allocator);

                    std::string body_type = "dynamic";
                    bool precise = true;
                    float gravity_scale = 1.0f;
                    float density = 1.0f;
                    float angularFriction = 0.3f;
                    float rotation = 0.0f;          // This value will ALWAYS be in radians. If needed for lua, convert it to degrees rads * (180.0f / b2_pi)
                    bool hasCollider = true;
                    bool has_trigger = true;
                    std::string colliderType = "box";
                    float width = 1.0f;
                    float height = 1.0f;
                    float radius = 0.5f;
                    float friction = 0.3f;
                    float bounciness = 0.3f;
                    std::string triggerType = "box";
                    float triggerWidth = 1.0f;
                    float triggerHeight = 1.0f;
                    float triggerRadius = 0.5f;
                }
                if (comp.first == "ParticleSystem")
                {
                    compObj.AddMember("x", actor->position.x, allocator);
                    compObj.AddMember("y", actor->position.y, allocator);
                }
                componentsObj.AddMember(
                    rapidjson::Value(comp.second.c_str(), allocator),  // string key
                    compObj,
                    allocator
                );
            }
            actorObj.AddMember("components", componentsObj, allocator);
        }

        actorsArray.PushBack(actorObj, allocator);
    }

    doc.AddMember("actors", actorsArray, allocator);

    // 4. Write to file
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);
    outFile << buffer.GetString();
    outFile.close();
}

void Editor::add_end_button(bool* done)
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x /2, 0), ImGuiCond_Always);         // Top-left corner
    ImGui::SetNextWindowSize(ImVec2(20, 20), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    ImGui::Begin("FloatingButton", nullptr, window_flags);
    if (ImGui::Button("->", ImVec2(30, 30)))
    {
        *done = true;
    }

    ImGui::End();
}

void Editor::Render_Editor_Actors()
{
    // Iterate through the list of actors and if they have render on, render their image.
    // If their image is not loaded, load it in. Then render them at their x,y, coordinates
    for (auto actorIt : currentSceneActorMap.at(currentSceneName))
    {
        if (actorIt->isRendering)
        {
            if (actorIt->textureName != "")
            {
                // Iterating through the actors in the current scene
                checkForImage(actorIt->textureName);

                // We have garunteed that the image is loaded at this point
                SDL_Texture* tex = loadedImages.at(actorIt->textureName);
                SDL_FRect texRect;
                Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);
                float haldWinX = ImGui::GetIO().DisplaySize.x / 2;
                float halfWinY = ImGui::GetIO().DisplaySize.y / 2;

                int newRotation = static_cast<int>(actorIt->rotation);
                ImageDrawRequest currentRequest = { actorIt->textureName, actorIt->position.x, actorIt->position.y, newRotation, actorIt->scaleX, actorIt->scaleY, actorIt->pivotX, actorIt->pivotY, static_cast<int>(actorIt->r), static_cast<int>(actorIt->g), static_cast<int>(actorIt->b), static_cast<int>(actorIt->a), actorIt->sorting_order , DrawPrioirtyNumber };
                DrawPrioirtyNumber++;

                DrawRequests.push_back(currentRequest);
            }
        }

    }
}

void Editor::checkForImage(std::string imageName)
{
    if (loadedImages.find(imageName) == loadedImages.end())
    {
        // image is not currently loaded in
        std::string fileName = "resources/images/" + imageName + ".png";
        if (std::filesystem::exists(fileName))
        {
            SDL_Texture* loadedImage = IMG_LoadTexture(renderer, fileName.c_str());
            loadedImages.insert({ imageName, loadedImage });
        }
        else
        {
            std::cout << "error: missing image " << imageName;
            exit(0);
        }
    }
}

void Editor::render_and_clear_image_queue()
{
    std::stable_sort(DrawRequests.begin(), DrawRequests.end(), &ImageRenderComparator);

    for (auto& request : DrawRequests)
    {
        const int pixelsPerMeter = 100;
        ImVec2 finalRenderingPosition = ImVec2(request.x, request.y);
        
        //cout << finalRenderingPosition.x << endl;;
        SDL_Texture* tex = loadedImages.at(request.imageName);
        SDL_FRect texRect;
        Helper::SDL_QueryTexture(tex, &texRect.w, &texRect.h);

        int flipMode = SDL_FLIP_NONE;
        if (request.scaleX < 0)
            flipMode |= SDL_FLIP_HORIZONTAL;
        if (request.scaleY < 0)
            flipMode |= SDL_FLIP_VERTICAL;

        float xScale = abs(request.scaleX);
        float yScale = abs(request.scaleY);

        texRect.w *= xScale;
        texRect.h *= yScale;

        SDL_FPoint pivotPoint = { (request.pivotX * texRect.w), (request.pivotY * texRect.h) };

        ImVec2 camDimensions;
        camDimensions.x = ImGui::GetIO().DisplaySize.x / 2;
        camDimensions.y = ImGui::GetIO().DisplaySize.y / 2;
        texRect.x = (finalRenderingPosition.x + camDimensions.x * 0.5f  - pivotPoint.x);
        texRect.y = (finalRenderingPosition.y + camDimensions.y * 0.5f  - pivotPoint.y);

        SDL_SetTextureColorMod(tex, request.r, request.g, request.b);
        SDL_SetTextureAlphaMod(tex, request.a);

        Helper::SDL_RenderCopyEx(1, request.imageName, renderer, tex, NULL, &texRect, request.rotationDegrees, &pivotPoint, static_cast<SDL_RendererFlip>(flipMode));


        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }

    SDL_RenderSetScale(renderer, 1, 1);

    DrawRequests.clear();
    DrawPrioirtyNumber = 0;
}

/*

*/












int fakeBegin()
{
    // Begin all of the editor logic
     // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.y
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
