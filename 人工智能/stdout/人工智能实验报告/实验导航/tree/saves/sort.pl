quicksort([X | Xs], Ys) <- partition(Xs, X, Littles, Bigs) & quicksort(Littles, Ls) & quicksort(Bigs, Bs) & append(Ls, [X | Bs], Ys).
quicksort([], []).

partition([X | Xs], Y, [X | Ls], Bs) <- X=<Y & partition(Xs, Y, Ls, Bs).
partition([X | Xs], Y, Ls, [X | Bs]) <- X > Y & partition(Xs, Y, Ls, Bs).

partition([], Y,[],[]).

append([X|Xs],Ys,[X|Zs]) <- append(Xs,Ys,Zs).
append([],Ys,Ys).

