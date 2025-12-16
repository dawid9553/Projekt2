# CMakeList.txt: projekt CMake dla elementu Projekt, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Projekt "Projekt.cpp" "Projekt.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Projekt PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.


include(FetchContent)
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.5
)
FetchContent_MakeAvailable(SDL2)
target_link_libraries(Projekt PRIVATE SDL2::SDL2main SDL2::SDL2-static)

target_include_directories(Projekt PRIVATE
        ${sdl2_SOURCE_DIR}/include
)
