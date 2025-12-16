# CMakeList.txt: projekt CMake dla elementu Projekt, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (Projekt "Projekt.cpp" "Projekt.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Projekt PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.
