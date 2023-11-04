SERV_NAME	=	irc_server
CLI_NAME	=	irc_client

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -lncurses
RM			=	rm
INC 		= 	inc/

SRC_DIR 	= srcs
BIN_DIR 	= bin
OBJ_DIR 	= obj

#relative to their respective directories
COMM_SRCS	= 	TCPSocket.cpp
SERV_SRCS	=	User.cpp channel.cpp server.cpp server_main.cpp
CLI_SRCS	=	tui.cpp client.cpp client_main.cpp

SERV_SRCS	:= ${addprefix server/, ${SERV_SRCS}}
CLI_SRCS	:= ${addprefix client/, ${CLI_SRCS}}

SERV_OBJS	=	${SERV_SRCS:.cpp=.o}
CLI_OBJS	=	${CLI_SRCS:.cpp=.o}
COMM_OBJS	=	${COMM_SRCS:.cpp=.o}

SERV_OBJS	:= ${addprefix $(OBJ_DIR)/, ${SERV_OBJS}}
CLI_OBJS	:= ${addprefix $(OBJ_DIR)/, ${CLI_OBJS}}
COMM_OBJS	:= ${addprefix $(OBJ_DIR)/, ${COMM_OBJS}}


all: setup $(BIN_DIR)/$(SERV_NAME) $(BIN_DIR)/$(CLI_NAME) $(COMM_OBJS)

$(BIN_DIR)/$(SERV_NAME):	$(SERV_OBJS) $(COMM_OBJS)
	${CXX} ${CXXFLAGS} $(SERV_OBJS) $(COMM_OBJS) -o $(BIN_DIR)/$(SERV_NAME)

$(BIN_DIR)/$(CLI_NAME):	$(CLI_OBJS) $(COMM_OBJS)
	${CXX} ${CXXFLAGS} $(CLI_OBJS) $(COMM_OBJS) -o $(BIN_DIR)/$(CLI_NAME)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	${CXX} ${CXXFLAGS} -I${INC} -g -c $^ -o $@

clean:
	${RM} $(SERV_OBJS) $(CLI_OBJS) $(COMM_OBJS)

fclean:		clean
	${RM} $(BIN_DIR)/*

setup:
	mkdir -p bin
	mkdir -p obj/client obj/server
	
re:			fclean all

.PHONY:	all clean fclean re
