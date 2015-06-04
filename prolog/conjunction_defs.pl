conjunction([]).
conjunction([H|T]) :- H, conjunction(T).
