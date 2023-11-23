SERV_NAME	=	irc_server
CLI_NAME	=	irc_client

CXX			=	g++
CXXFLAGS	=	-Wall -Wextra
RM			=	rm
LINK = -lncurses
INC 		= 	inc/

SRC_DIR 	= srcs
BIN_DIR 	= bin
OBJ_DIR 	= obj

#relative to their respective directories
COMM_SRCS	= 	srcs/TCPSocket.cpp
SERV_SRCS	=	User.cpp channel.cpp commands.cpp server.cpp server_main.cpp
CLI_SRCS	=	tui.cpp client.cpp client_main.cpp

SERV_SRCS	:= ${addprefix srcs/server/, ${SERV_SRCS}}
CLI_SRCS	:= ${addprefix srcs/client/, ${CLI_SRCS}}

all: setup $(BIN_DIR)/$(SERV_NAME) $(BIN_DIR)/$(CLI_NAME)

$(BIN_DIR)/$(SERV_NAME): ${SERV_SRCS} ${COMM_SRCS}
	${CXX} ${CXXFLAGS} $(SERV_SRCS) $(COMM_SRCS) -o $(BIN_DIR)/$(SERV_NAME)

$(BIN_DIR)/$(CLI_NAME):	${CLI_SRCS} ${COMM_SRCS}
	${CXX} ${CXXFLAGS} $(CLI_SRCS) $(COMM_SRCS) -o $(BIN_DIR)/$(CLI_NAME) $(LINK)


clean:
	${RM} $(BIN_DIR)/*

setup:
	mkdir -p bin	
