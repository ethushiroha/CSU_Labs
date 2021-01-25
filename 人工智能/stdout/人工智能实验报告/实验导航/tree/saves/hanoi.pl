% file: hanoi.pl

% hanoi(N,A,B,C,Moves) <- is true if Moves is a sequence of
% moves for solving the Towers of Hanoi puzzle with N
% disks and three pegs, A, B, and C.

hanoi(s(0),A,B,C,[ to(A,B) ]).
hanoi(s(N), A,B,C, Moves ) <- hanoi(N,A,C,B,Ms1) &
	hanoi(N,C,B,A,Ms2) & 
	append(Ms1,[to(A,B)|Ms2],Moves).
hanoi(0,A,B,C,[]).

append([],Z,Z).
append([A|X],Y,[A|Z]) <-
   append(X,Y,Z).

