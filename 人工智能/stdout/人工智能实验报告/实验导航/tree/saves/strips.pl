% DEFINITION OF DELIVERY ROBOT WORLD IN STRIPS NOTATION

% ACTIONS
% move(Ag,Pos,Pos_1) is the action of Ag moving from Pos to Pos_1

preconditions(move(Ag,Pos,Pos_1),
    [autonomous(Ag), adjacent(Pos,Pos_1), sitting_at(Ag,Pos)]).
achieves(move(Ag,Pos,Pos_1),sitting_at(Ag,Pos_1)).
deletes(move(Ag,Pos,Pos_1),sitting_at(Ag,Pos)).

% pickup(Ag,Obj,Pos) is the action of agent Ag picking up Obj.
preconditions(pickup(Ag,Obj),
    [autonomous(Ag), Ag \= Obj, location(Pos),
                       sitting_at(Obj,Pos), at(Ag,Pos) ]).
achieves(pickup(Ag,Obj), carrying(Ag,Obj)).
deletes(pickup(Ag,Obj), sitting_at(Obj,Pos)).

% putdown(Ag,Obj,Pos)
preconditions(putdown(Ag,Obj,Pos), 
    [autonomous(Ag),  Ag \= Obj, at(Ag,Pos), carrying(Ag,Obj)]).
achieves(putdown(Ag,Obj,Pos),sitting_at(Obj,Pos)).
deletes(putdown(Ag,Obj,Pos),carrying(Ag,Obj)).

% unlock(Ag,Door)
preconditions(unlock(Ag,Door),
    [autonomous(Ag), blocks(Door,P_1,_), opens(Key,Door),  
        carrying(Ag,Key), at(Ag,P_1)]).
achieves(unlock(Ag,Door),unlocked(Door)).

% PRIMITIVE RELATIONS
primitive(carrying(_,_)).
primitive(sitting_at(_,_)).
primitive(unlocked(_)).

% DERIVED RELATIONS

clause(at(Obj,Pos),[sitting_at(Obj,Pos)] ).
clause(at(Obj,Pos),[autonomous(Ag), Ag \= Obj, carrying(Ag,Obj), at(Ag,Pos)]).

clause(location(o109),[]).
clause(location(o103),[]).
clause(location(storage),[]).
clause(location(o111),[]).
clause(location(mail),[]).
clause(location(lab2),[]).


clause(adjacent(o109,o103),[]).
clause(adjacent(o103,o109),[]).
clause(adjacent(o109,storage),[]).
clause(adjacent(storage,o109),[]).
clause(adjacent(o109,o111),[]).
clause(adjacent(o111,o109),[]).
clause(adjacent(o103,mail),[]).
clause(adjacent(mail,o103),[]).
clause(adjacent(lab2,o109),[]).
clause(adjacent(P_1,P_2), [blocks(Door,P_1,P_2), unlocked(Door)]).
clause(blocks(door1,o103,lab2),[]).
clause(opens(k1,door1),[]).
clause(autonomous(rob),[]).

% INITIAL SITUATION
holds(sitting_at(rob,o109),init).
holds(sitting_at(parcel,storage),init).
holds(sitting_at(k1,mail),init).

achieves(init,X) <-
   holds(X,init).

% A SIMPLE STRIPS PLANNER USING STRIPS NOTATION
 
%achieve_all(Gs,S0,S1,U0,U1) is true if every element of list Gs can be
%   achieved  going from state S0 to state S1.
% U0 is the bound on the number of actions going into achieve_all
%   and U1 is the limit coming out
 
achieve_all([],World,World,U,U).
achieve_all(Gs,W0,W2,U0,U2) <-
   remove(G1,Gs,Gr) &
   achieve(G1,W0,W1,U0,U1) &
   achieve_all(Gr,W1,W2,U1,U2).
 
% achieve(G,S0,S1) is true if goal G can be achieved going from S0 to S1
 
achieve(G,W,W,U,U) <-              
   true_in(G,W).
achieve(G,W0,W1,U0,U1) <-          
   clause(G, Body) &
   achieve_all(Body,W0,W1,U0,U1).
achieve(A \= B,W,W,U,U) <-         
   A \= B.
achieve(G,W0,do(Act,W1),U0,U2) <- 
   U0>0 &
   U1 is U0-1 &
   achieves(Act,G) &
   preconditions(Act,PreAct) &
   achieve_all(PreAct,W0,W1,U1,U2).
 
% remove(Elt,List,RemainingElts).
remove(X,[X|Y],Y).
 
% true_in(Goal,State) is true if Goal is true in State.
true_in(G,init) <-
   holds(G,init).
true_in(G,do(A,_)) <-
   achieves(A,G).
true_in(G,do(A,S)) <-
   true_in(G,S) &
   ~ deletes(A,G).
 
% TRY THE FOLLOWING QUERIES with delrob_strips.pl:
% ask achieve(carrying(rob,k1),init,S,10,_).
% ask achieve(at(k1,lab2),init,S,6,N).
% ask achieve_all([carrying(rob,parcel),sitting_at(rob,lab2)],init,S,10,N).
% ask achieve_all([sitting_at(rob,lab2),carrying(rob,parcel)],init,S,10,N).
%    is the plan returned correct?

