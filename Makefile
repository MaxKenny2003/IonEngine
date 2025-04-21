main:
	clang++ -O3 -std=c++17 *.cpp -o game_engine_linux -I./include -I./src -I/include/box2d -I./ThirdParty -I./ThirdParty/LuaBridge -I./ -I./ThirdParty/LuaBridge/detail -I./ThirdParty/glm -I./ThirdParty/glm/gtx -I./ThirdParty/rapidjson-1.1.0 -I./ThirdParty/SDL2/ -I./ThirdParty/SDL2_image/ -I./ThirdParty/SDL2_mixer/ -I./ThirdParty/SDL2_ttf/ -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -llua5.4
