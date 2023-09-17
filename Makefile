NAME		=	irc

CXX			=	c++
RM			=	rm
LIB 		=	# if we decide to have custom lib, add libname here
INC 		= 	inc/
SERV_SRCS	=	srcs/server/server.cpp
CLI_SRCS	=	srcs/client/client.cpp
SERV_OBJS	=	${SERV_SRCS:.cpp=.o}
CLI_OBJS	=	${CLI_SRCS:.cpp=.o}
CXXFLAGS	=	-Wall -Wextra -Werror

.cpp.o:
			${CXX} ${CXXFLAGS} -I${INC} -g -c $< -o ${<:.cpp=.o}

all:		$(NAME)

$(NAME):	$(SERV_OBJS) $(CLI_OBJS)
			${CC} ${CFLAGS} $(SERV_OBJS) -o irc_server
			${CC} ${CFLAGS} $(CLI_OBJS) -o irc_client

clean:
			${RM} $(SERV_OBJS) $(CLI_OBJS) irc_client irc_server

fclean:		clean

re:			fclean all

.PHONY: 	bonus all clean fclean re