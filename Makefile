##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Makefile
##

TARGET			=	zappy_gui

APPIMAGE_DIR	=	App

CXX				=	g++
CXXFLAGS		=	-Wall -Wextra -std=gnu++20
LDFLAGS			=	-lsfml-graphics -lsfml-window -lsfml-system -lGL
INCLUDES		=	-ISource -ISource/Libraries
FLAGS			=	$(CXXFLAGS) $(LDFLAGS) $(INCLUDES)

SOURCES			=	$(shell find Source -type f -iname "*.cpp")
OBJECTS			=	$(SOURCES:.cpp=.o)

TEST_SOURCES	=	$(filter-out Source/Main.cpp, $(SOURCES)) \
					$(shell find Tests -type f -iname "*.cpp")
TEST_OBJECTS	=	$(TEST_SOURCES:.cpp=.o)

TOTAL			:=	$(words $(SOURCES))

RESET			=	\033[0m
BOLD			=	\033[1m
GREEN			=	\033[32m
BLUE			=	\033[34m
YELLOW			=	\033[33m

all: $(TARGET)

show_infos:
	@printf "$(BLUE)$(BOLD)[INFO]$(RESET) Compilation flags: $(FLAGS)\n"
	@printf "$(BLUE)$(BOLD)[INFO]$(RESET) Total source files: $(TOTAL)\n"

%.o: %.cpp
	@printf "$(BLUE)[CXX]$(RESET) $(BOLD)Compiling$(RESET) $<\n"
	@$(CXX) -c $< -o $@ $(FLAGS)

$(TARGET): show_infos $(OBJECTS)
	@printf "$(GREEN)[LINK]$(RESET) $(BOLD)Linking$(RESET) $@\n"
	@$(CXX) -o $(TARGET) $(OBJECTS) $(FLAGS)
	@printf "$(GREEN)$(BOLD)Build completed successfully!$(RESET)\n"

tests: LDFLAGS += -lcriterion --coverage
tests: unit_tests

unit_tests: $(TEST_OBJECTS)
	@printf "$(GREEN)[LINK]$(RESET) $(BOLD)Linking$(RESET) $@\n"
	@$(CXX) -o unit_tests $(TEST_OBJECTS) $(FLAGS)
	@printf "$(GREEN)$(BOLD)Build completed successfully!$(RESET)\n"

tests_run: tests
	@printf "$(YELLOW)[TESTS]$(RESET) $(BOLD)Running tests$(RESET)\n"
	@./unit_tests

clean:
	@printf "$(YELLOW)[CLEAN]$(RESET) $(BOLD)Removing object files$(RESET)\n"
	@find -type f -iname "*.o" -delete
	@find -type f -iname "*.d" -delete

fclean: clean
	@printf "$(YELLOW)[FCLEAN]$(RESET) $(BOLD)Removing target$(RESET)\n"
	@rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re tests unit_tests tests_run

###############################################################################
# AppImage creation
###############################################################################

download-x86_64:
	@if [ ! -f linuxdeploy-x86_64.AppImage ]; then \
		printf "$(YELLOW)[APPIMAGE]$(RESET) $(BOLD)Downloading LinuxDeploy$(RESET)\n"; \
		echo "Downloading linuxdeploy-x86_64.AppImage..."; \
		curl -L -o linuxdeploy-x86_64.AppImage https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage; \
		chmod +x linuxdeploy-x86_64.AppImage; \
	fi

build-x86_64: download-x86_64 all
	@printf "$(YELLOW)[APPIMAGE]$(RESET) $(BOLD)Preparing AppImage$(RESET)\n"
	@rm -rf $(APPIMAGE_DIR)

	@mkdir -p $(APPIMAGE_DIR)/usr/bin
	@mkdir -p $(APPIMAGE_DIR)/usr/share/applications
	@mkdir -p $(APPIMAGE_DIR)/usr/share/icons/hicolor/256x256/apps

	@cp $(TARGET) $(APPIMAGE_DIR)/usr/bin/
	@cp -r Assets $(APPIMAGE_DIR)/usr/share/

	@cp Assets/zappy_gui.png $(APPIMAGE_DIR)/usr/share/icons/hicolor/256x256/apps/
	@cp Assets/zappy_gui.png $(APPIMAGE_DIR)/

	@echo "[Desktop Entry]\n\
	Name=zappy_gui\n\
	Exec=zappy_gui\n\
	Icon=zappy_gui\n\
	Type=Application\n\
	Categories=Game;" > $(APPIMAGE_DIR)/usr/share/applications/zappy_gui.desktop

	@printf "$(YELLOW)[APPIMAGE]$(RESET) $(BOLD)Building AppImage$(RESET)\n"
	@./linuxdeploy-x86_64.AppImage --appdir $(APPIMAGE_DIR) --output appimage
