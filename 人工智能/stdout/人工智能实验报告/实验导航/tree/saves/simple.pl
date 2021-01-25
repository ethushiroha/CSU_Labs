son(X,Y) <- father(Y,X) & male(X).
son(X,Y) <- mother(Y,X) & male(X).
daughter(X,Y) <- father(Y,X) & female(X).
daughter(X,Y) <- mother(Y,X) & female(X).
grandfather(X,Y) <- father(X,Z) & father(Z,Y).
grandfather(X,Y) <- father(X,Z) & mother(Z,Y).

% rules for father-2

father(terach,abraham).
father(terach,nachor).
father(terach,haran).
father(abraham,isaac).
father(haran,milcah).
father(haran,yiscah).
father(haran,lot).

mother(sarah,isaac).

male(terach).
male(abraham).
male(nachor).
male(haran).
male(isaac).
male(lot).

female(sarah).
female(milcah).
female(yiscah).

