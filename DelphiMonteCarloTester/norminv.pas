unit NormInv;

interface

function NormaleInverse(q: double; Mu: double; Sigma: double ): double;

implementation

uses
  math;

const c0 = 2.515517;
const c1 = 0.802853;
const c2 = 0.010328;
const d1 = 1.432788;
const d2 = 0.189269;
const d3 = 0.001308;

function NormaleInverse(q: double; Mu: double; Sigma: double ): double;
var
  x: double;
  p: double;
  t: double;
  l: double;
begin
  if(q = 0.5) then
     result := Mu
  else
  begin
    q := 1.0 - q;
    if(q > 0.0) and (q < 0.5) then
      p := q
    else
      p := 1 - q;

    l := ln(1/Power(p,2));
    if( l<0 ) then
      t := 0
     else
      t := sqrt(l);
    x := t - ( c0 + c1*t + c2*Power(t,2))/(1+d1*t+d2*Power(t,2) + d3*Power(t,3));

    if(q > 0.5) then
      x := -x;
    result := (x * Sigma) + Mu;
  end;
end;

end.



