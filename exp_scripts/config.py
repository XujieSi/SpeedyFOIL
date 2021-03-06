MODE = "-GS"

T_GRAPH = "graph.t"
T_MATH = "math.t"
T_PA= "pa.t"
T_LIST = "list.t"
T_KD = "knowledge.t"
T_ALL = "all.t"

TMPL = "template"
DFILE =  "dfile"
K = "K"
M = "M"

downcast = "downcast"
polysite = "polysite"
path = "path"
p25_modref = "p25_modref"
p50_modref = "p50_modref"
ancestor = "ancestor"
abduce = "abduce"
animals = "animals"
apisan = "apisan"
andersen = "andersen"
sgen = "samegen"
escape = "escape"
uf = "uf"
reverse = "reverse"
sort = "sort"
member = "member"
scc = "scc"
ncm = "ncm"
gcd = "gcd"
btree = "btree"
ackermann = "ackermann"
perm = "perm"

full_modref = "full_modref"
modref = "modref"

pa_tasks = [downcast, polysite, escape, full_modref,  andersen,]
kd_tasks = [ancestor, abduce, animals, sgen, apisan, ]
math_tasks = [ackermann, ncm, gcd,]
graph_tasks = [ path,  scc, btree ] # uf
list_tasks = [ member, sort, perm, reverse ]

tasks = pa_tasks + math_tasks + graph_tasks + list_tasks + kd_tasks

#tasks = [full_modref]


def get_tmpl(t):
    return T_ALL
    if t not in tasks:
        print t, "is not a valid tasks"
    if t in pa_tasks:
        return T_PA
    elif t in math_tasks:
        return T_MATH
    elif t in graph_tasks:
        return T_GRAPH
    elif t in list_tasks:
        return T_LIST
    elif t in kd_tasks:
        return T_KD

    print "Unknown category for ", t

    return T_ALL



benchmarks = {
    downcast : { K : 1, DFILE : "downcast.d", },
    polysite : { K : 1, DFILE : "polysite.d", },
    p25_modref   : { K : 2, DFILE : "p25_modref.d", },
    p50_modref   : { K : 2, DFILE : "p50_modref.d", },
    full_modref   : { K : 2, DFILE : "full_modref.d", },
    apisan   : { K : 2, DFILE : "apisan.d", },
    andersen : { K : 4, DFILE : "aws_andersen.d", },
    escape : { K:2, DFILE : "escape.d"},

    path     : { K : 2, DFILE : "path.d", },
    ancestor : { K : 2, DFILE : "ancestor_new.d", },
    abduce   : { K : 2, M : 3,  DFILE : "abduce.d", },
    animals  : { K : 1, DFILE : "animals.d", },
    sgen     : { K : 2, M : 3, DFILE : "sgen.d"},
    scc     : { K : 2, M : 3, DFILE : "scc.d"},


    ackermann : { K : 3,  DFILE : "ackermann.d", },
    gcd : { K : 3,  DFILE : "gcd.d", },
    ncm : { K:2, DFILE : "ncm.d", },


    member : { K:2, DFILE : "member.d", },
    sort :  { K:3, M : 4, DFILE : "sort.d", },
    perm : { K:2, DFILE : "perm.d", },
    uf : { K:4, M : 4, DFILE :  "uf.d"},
    btree : {K:2,  DFILE : "btree.d" },
    reverse : { K:2,  DFILE: "list.d"},

    #full_modref : { K : 2, DFILE : "full_modref.d"},
}

alps_result = {
    abduce : 12,
    ackermann : 6,
    ancestor : 17,
    andersen : 17,
    animals : 15,
    apisan : 5,
    downcast : 9,
    escape : 11,
    gcd : 6,
    member : 1,
    ncm : 6,
    modref : 15,
    path : 5,
    perm : 6,
    polysite : 7,
    reverse : 7,
    sgen : 10,
    scc : 9,
    sort : 10,
    uf : 9,
}
