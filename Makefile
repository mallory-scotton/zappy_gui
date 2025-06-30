##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Makefile
##

TARGET			=	zappy_gui

CXX				=	g++
CXXFLAGS		=	-Wall -Wextra -std=gnu++20 -static
LDFLAGS			=
INCLUDES		=	-ISource
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
