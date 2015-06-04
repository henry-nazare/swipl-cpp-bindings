compile_string(Source, String) :-
  new_memory_file(D),
  open_memory_file(D, write, S),
  format(S, String, []),
  close(S),
  open_memory_file(D, read, R, [free_on_close(true)]),
  load_files(Source, [stream(R)]),
  close(R).

