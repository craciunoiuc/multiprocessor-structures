function lab2
  x = 1:0.1:32;
  
  [ax, h1, h2] = plotyy(x, [En_output(x); Un_output(x)],
                        x, [Sn_output(x); Rn_output(x); Qn_output(x)]);
  hold on;

  set(ax, {"ycolor"}, {"r"; "b"});
  
  legend(["En"; "Un"; "Sn"; "Rn"; "Qn"]);
  legend("location", "northwestoutside");
  legend("boxoff");

  title("Plot that draws all curves");

  xlabel("n");
  ylabel(ax(1), "E(n) U(n)");
  ylabel(ax(2), "S(n), R(n), Q(n)");

  hold off;
endfunction

function ret = O1_input (n)
  ret = n.^3;
endfunction

function ret = T1_input (n)
  ret = n.^3;
endfunction

function ret = On_input (n)
  ret = n.^3 + n.^2 .* log2(n);
endfunction

function ret = Tn_input (n)
  ret = 4 * n.^3 ./ (n + 3);
endfunction

function ret = Sn_output (n)
  ret = T1_input(n) ./ Tn_input(n);
endfunction

function ret = En_output (n)
  ret = T1_input(n) ./ (n .* Tn_input(n));
endfunction

function ret = Rn_output (n)
  ret = On_input(n) ./ O1_input(n);
endfunction

function ret = Un_output (n)
  ret = On_input(n) ./ (n .* Tn_input(n));
endfunction

function ret = Qn_output (n)
  ret = T1_input(n).^3 ./ (n .* Tn_input(n).^2 .* On_input(n));
endfunction
