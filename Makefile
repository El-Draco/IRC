NAME		=	irc

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra
RM			=	rm
LIB 		=	# if we decide to have custom lib, add libname here
INC 		= 	inc/

SRC_DIR = srcs
BIN_DIR = bin
OBJ_DIR = obj

#relative to their respective directories
SERV_SRCS	=	server.cpp main_server.cpp test/test.cpp
CLI_SRCS	=	client.cpp main_client.cpp

SERV_SRCS := ${addprefix server/, ${SERV_SRCS}}
CLI_SRCS := ${addprefix client/, ${CLI_SRCS}}

SERV_OBJS	=	${SERV_SRCS:.cpp=.o}
CLI_OBJS	=	${CLI_SRCS:.cpp=.o}

SERV_OBJS := ${addprefix $(OBJ_DIR)/, ${SERV_OBJS}}
CLI_OBJS := ${addprefix $(OBJ_DIR)/, ${CLI_OBJS}}

all: $(NAME)

$(NAME):	$(SERV_OBJS) $(CLI_OBJS)
	${CXX} ${CXXFLAGS} $(SERV_OBJS) -o $(BIN_DIR)/irc_server
	${CXX} ${CXXFLAGS} $(CLI_OBJS) -o $(BIN_DIR)/irc_client

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	${CXX} ${CXXFLAGS} -I${INC} -g -c $^ -o $@

clean:
	${RM} $(SERV_OBJS) $(CLI_OBJS) $(BIN_DIR)/*

fclean:		clean

re:			fclean all

setup:
	mkdir -p bin
	mkdir -p obj/client obj/server

.PHONY: 	bonus all clean fclean re
