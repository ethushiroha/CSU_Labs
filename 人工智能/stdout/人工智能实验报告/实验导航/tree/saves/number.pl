% number(X) is true if X is a number

number(0).
number(s(X)) <- number(X).

% plus(X,Y,Z) is true if X + Y = Z

plus(0,X,X) <- number(X).
plus(s(X),Y,s(Z)) <- plus(X,Y,Z).

% minus(X,Y,Z) is true if X - Y =Z

minus(X,0,X) <- number(X).
minus(s(X),s(Y),Z) <- minus(X,Y,Z).

% times(X,Y,Z) is true if X * Y = Z

times(X,0,0) <- number(X).
times(X,s(Y),Z) <- times(X,Y,Z1) & plus(X,Z1,Z).

% pow(X,Y,Z) is true if X^Y = Z

pow(X,0,s(0)) <- number(X).
pow(X,s(Y),Z) <-pow(X,Y,Z1) & times(X,Z1,Z).

