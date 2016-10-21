#include "query.h"

#include <vector>
#include <sstream>
#include <algorithm>

namespace SpeedyFOIL {

namespace {

std::vector<int> extract_bv_values(z3::expr& E) {
	std::vector<int> v;

	int m_args = E.num_args();
	for (int j = 0; j < m_args; ++j) {
		z3::expr var_val = E.arg(j);

		v.push_back( var_val.arg(1).get_numeral_int() );
	}

	return v;
}

const char* str(int x) {
	std::stringstream ss;
	ss << "v" << x;
	return ss.str().c_str();
}

} // end of anonymous namespace


FixedPoint::FixedPoint(ContextManager* pCM) : pContext(&(pCM->C)), defaultS(pContext->str_symbol("defaultS")) {
	z3_fp = Z3_mk_fixedpoint( *pContext );
	Z3_fixedpoint_inc_ref(*pContext,z3_fp);

	recent = Z3_L_UNDEF;
	// register relation definitions
	for (auto pr : pCM->funcMap) {
		Z3_fixedpoint_register_relation(*pContext, z3_fp, pr.second);
	}

	pCM->appendEDBConstr(z3_fp);
}

void FixedPoint::add_rules(std::vector<z3::expr>& rules) {
	for (z3::expr& r : rules) {
		Z3_fixedpoint_add_rule(*pContext, z3_fp, r, defaultS);
	}
}

void FixedPoint::add_rule(z3::expr rule) {
	Z3_fixedpoint_add_rule(*pContext, z3_fp, rule, defaultS);
}

bool FixedPoint::query(Z3_ast Q) {
	recent = Z3_fixedpoint_query( *pContext, z3_fp, Q);
	if(recent == Z3_L_UNDEF) {
		std::cerr << "query result is UNDEF" << std::endl;
	}
	return recent == Z3_L_TRUE;
}

z3::expr FixedPoint::get_answer(){
	if(recent != Z3_L_TRUE) {
		std::cerr << "ERROR: cannot get_answer unless recent status is true" << std::endl;
	}

	Z3_ast ast_res = Z3_fixedpoint_get_answer( *pContext, z3_fp);
	z3::expr detailed_res( *pContext, ast_res);

	return detailed_res;
}

z3::expr QueryEngine::build_func_constr(z3::context& context,
		std::map<int, z3::expr>& z3_vars,
		std::pair<Relation, std::vector<int>>& pair) {

	Relation rel = pair.first;

	auto it = cm_ptr->funcMap.find(rel);
	if (it == cm_ptr->funcMap.end()) {
		std::cerr << "ERROR, QueryEngine::helper got unknown relation : "
				<< rel->Name << std::endl;
	}

	z3::func_decl f = it->second;

	z3::expr_vector params(context);
	for (int x : pair.second) {
		auto it2 = z3_vars.find(x);
		if (it2 == z3_vars.end()) {
			std::cerr << "ERROR, QueryEngine::helper got undefined var: "
					<< str(x) << std::endl;
		}

		params.push_back(it2->second);
	}

	return f(params);
}

void QueryEngine::parse_and_update(z3::expr& E, int idx) {
	std::string app_name = E.decl().name().str();
	if (app_name == "or") {
		int n_args = E.num_args();
		for (int i = 0; i < n_args; ++i) {
			z3::expr e = E.arg(i);
			std::vector<int> tp = extract_bv_values(e);

			// prepend idx
			tp.insert(tp.begin(), idx);
			++vote_stats[tp];
		}
	} else if (app_name == "and") {
		std::vector<int> tp = extract_bv_values(E);
		tp.insert(tp.begin(), idx);
		++vote_stats[tp];
	} else {
		std::cerr << "unknown application name: " << app_name << std::endl;
	}
}

void QueryEngine::queryIDBs(
		std::set<std::pair<Relation, std::vector<int>>>& queries, FixedPoint& fp) {
	z3::context& context = cm_ptr->C;
	z3::sort bv_sort = context.bv_sort(cm_ptr->MaxBits);

	// Query
	for (auto Q : queries) {
		Relation rel = Q.first;
		z3::func_decl f = cm_ptr->funcMap.find(rel)->second;

		std::set<int> vars(Q.second.begin(), Q.second.end());

		z3::expr_vector ex_vs(context);
		std::map<int, z3::expr> z3_vars;
		for (int x : vars) {
			z3::expr vx = context.constant(str(x), bv_sort);
			z3_vars.insert( std::make_pair(x, vx) );
			ex_vs.push_back(vx);
		}

		z3::expr_vector params(context);
		for (int x : Q.second) {
			auto it = z3_vars.find(x);
			params.push_back( it->second );
		}

		if ( fp.query(z3::exists(ex_vs, f(params))) ){
			z3::expr detailed_res = fp.get_answer();
			int idb_index = dp_ptr->findIDBIndex(rel);
			parse_and_update(detailed_res, idb_index);

		}
		else {
			++warn_ct;
		}
	}

}


FixedPoint QueryEngine::prepare(const DatalogProgram & dp,
		std::set<std::pair<Relation, std::vector<int>>>& queries) {
	z3::context& context = cm_ptr->C;
	z3::sort bv_sort = context.bv_sort(cm_ptr->MaxBits);

	std::vector<z3::expr> z3_rs;

	std::vector<ConcreteRule> rs = dp_ptr->getConcreteRules( dp );

	for (ConcreteRule& r : rs) {
		// define vars
		std::set<int> vars;
		for (auto pr : r) {
			for (int x : pr.second) {
				vars.insert(x);
			}
		}

		std::map<int, z3::expr> z3_vars;
		z3::expr_vector all_vs(context);
		for (int x : vars) {
			//z3::expr va = c.constant("a", bv3);
			z3::expr vx = context.constant(str(x), bv_sort);
			z3_vars.insert( std::make_pair(x, vx) );
			all_vs.push_back(vx);
		}

		z3::expr head = build_func_constr(context, z3_vars, r[0]);

		queries.insert(r[0]);

		z3::expr_vector ev(context);
		const int n = r.size();
		for (int i = 1; i < n; ++i) {
			z3::expr e = build_func_constr(context, z3_vars, r[i]);
			ev.push_back(e);
		}
		// z3::expr r2 = z3::forall(va, vb, vc, z3::implies( z3::mk_and(ev), path(va,vc) ));

		z3::expr rule = z3::forall(all_vs, z3::implies(z3::mk_and(ev), head));

		z3_rs.push_back(rule);
	}

	FixedPoint fp(cm_ptr);
	fp.add_rules(z3_rs);

	return fp;
}

void QueryEngine::execute(const DatalogProgram& dp) {
	std::set<std::pair<Relation, std::vector<int>>> queries;
	FixedPoint fp = prepare(dp, queries);
	queryIDBs(queries, fp);
}

bool QueryEngine::test(const DatalogProgram& dp, std::vector<int>& Q) {
	std::set<std::pair<Relation, std::vector<int>>> queries;
	FixedPoint fp = prepare(dp, queries);

	const int idb_index = Q[0];
	Relation rel = dp_ptr->idbRules[idb_index].rel.pRel;
	auto it = cm_ptr->funcMap.find(rel);
	if (it == cm_ptr->funcMap.end()) {
		std::cerr << "ERROR, QueryEngine::test got unknown relation : "
				<< rel->Name << std::endl;
	}
	z3::func_decl f = it->second;


	z3::context& context = cm_ptr->C;
	z3::expr_vector params( context);
	const int sz = Q.size();
	for(int i=1; i< sz; ++i) {
		z3::expr val = context.bv_val( Q[i] ,cm_ptr->MaxBits);
		params.push_back(val);
	}

	return fp.query( f(params) );
}

std::vector<int> QueryEngine::execute_one_round() {

	vote_stats.clear();
	warn_ct = 0;

	int i=0;
	auto it = dp_ptr->Gs.begin();
	while(it != dp_ptr->Gs.end()) {
		execute(*it);
		++it;
		++i;

		if(i%500 == 0) {
			std::cout << "i = " << i << ", warn_ct = " << warn_ct << std::endl;
			break;
		}
		//break;
	}

	std::cout << "warn_ct = " << warn_ct << std::endl;
	std::cout << "\ntuple stats: \n";
	for (auto pr : vote_stats) {
		std::cout << pr.second << " votes for ";
		for (int x : pr.first)
			std::cout << x << " ";
		std::cout << std::endl;
	}

	//const int ideal = dp_ptr->Gs.size() / 2;
	const int ideal = 500/2;

	int best = ideal;

	std::set<int> votes;
	std::vector<int> question;
	for (auto pr : vote_stats) {
		votes.insert(pr.second);

		int dis = pr.second - ideal;
		if(dis < 0) dis =-dis;

		if(dis < best) {
			question = pr.first;
			best = dis;
		}
	}


	std::cout << "best distance : " << best << std::endl;

	if(votes.size() == 1) {
		if( *votes.begin() == dp_ptr->Gs.size() ) {
			// converged !!!
			return std::vector<int>();
		}
	}

	std::copy(question.begin(), question.end(), std::ostream_iterator<int>(std::cout, ", ") );
	std::cout << std::endl;

	return question;

}

void QueryEngine::work() {

	int round = 0;
	while (true) {
		++round;

		std::vector<int> Q = execute_one_round();

		if(Q.size() == 0) {
			break;
		}

		bool positive = dp_ptr->ask(Q);

		if (positive) {
			// any general program that cannot cover Q should be eliminated.
			std::set<int> ids_to_remove;
			for(const DatalogProgram& x : dp_ptr->Gs) {
				if(test(x, Q) == false){
					ids_to_remove.insert( x.prog_id );

					if (ids_to_remove.size() % 1000 == 0) {
						std::cout << "remove.size = " << ids_to_remove.size() << std::endl
								<< "The following program will be eliminated by: ";
						std::copy(Q.begin(), Q.end(),
								std::ostream_iterator<int>(std::cout, ", "));
						std::cout << std::endl;

						std::cout << dp_ptr->str(x) << std::endl;
					}
				}
			}


			// refine specific program
		} else {
			// any general program that can cover Q needs refinement


			// eliminate specific program covering Q
		}

		break;
	}

	std::cout << "converged at round: " << round << std::endl;

}

} // end of namepsace SpeedyFOIL
