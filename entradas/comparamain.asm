mod1: begin
mod2: extern
public A
public b
public C
public AB
public CB
public cabo
troca_ab: extern
troca_cb: extern
troca_ab2: extern
input a
input b
input c
load a
sub b
jmpp troca_ab
ab: load b
sub c
jmpp troca_cb
cb: load a
sub b
jmpp troca_ab2
cabo: stop
a: space
b: space
c: space
end