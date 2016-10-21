#ifndef DATALOG_H
#define DATALOG_H

#include "template.h"
#include "matching.h"

#include <vector>

namespace SpeedyFOIL {

typedef std::vector< std::pair<Relation, std::vector<int>> > ConcreteRule;

struct DPManager;
struct DatalogProgram;
struct IDBTR;

// templates and rules for an IDB
struct IDBTR {
	TRelation rel;
	TemplateManager tm;
	std::vector<IClause> rules;

	std::map<int, std::set<int>> tmpl2rules;
	//std::set<std::set<int> > mostG;
	//std::set<std::set<int> > mostS;

	IDBTR(const TRelation & r) : rel(r){}

	IDBTR(IDBTR&& idb) noexcept :
			rel(idb.rel),
			tm(std::move(idb.tm)),
			rules(std::move(idb.rules)),
			tmpl2rules(std::move(idb.tmpl2rules)) {}
			//mostG(std::move(idb.mostG)),
			//mostS(std::move(idb.mostS)) {}

	IDBTR& operator = (IDBTR&& idb) noexcept {
		rel = idb.rel;

		tm = std::move(idb.tm);
		rules = std::move(idb.rules);
		tmpl2rules = std::move(idb.tmpl2rules);
		//mostG = std::move(idb.mostG);
		//mostS = std::move(idb.mostS);

		return *this;
	}

	void init();
	std::set<int> extractRules(const std::set<int>&) const;

	std::vector<std::set<int>> chooseK(int k, bool general) const;

	std::set<int> generalize(int rule_index) const;
	std::set<int> specialize(int rule_index) const;

};

struct DatalogProgram {
	static int programCt;

	const DPManager* dpm;
	int prog_id;
	std::map<int, std::set<int>> state;

	DatalogProgram(const DPManager* p) : prog_id(++programCt), dpm(p) {}
	DatalogProgram(const DatalogProgram&) = delete;

	DatalogProgram(DatalogProgram&& dp) noexcept : prog_id(dp.prog_id), dpm(dp.dpm), state(std::move(dp.state)) {
		dp.prog_id = -1;
		dp.dpm = nullptr;
	}

	DatalogProgram& operator = (DatalogProgram&& dp)  = delete;

	bool operator < (const DatalogProgram& dp) const {
		return prog_id < dp.prog_id;
	}

};


struct IDBValue {
	TRelation rel;
	std::set< std::vector<int> > pos;

	std::set< std::vector<int> > asked_pos;
	std::set< std::vector<int> > asked_neg;

	IDBValue(const TRelation& r) : rel(r) {}

	IDBValue(const IDBValue&) = delete;

	IDBValue(IDBValue&& iv) noexcept:
			rel(iv.rel),
			pos(std::move(iv.pos)),
			asked_pos(std::move(iv.asked_pos)),
			asked_neg(std::move(iv.asked_neg)){}

	IDBValue operator=(IDBValue&&) = delete;

};


struct DPManager {
	const Matching& M;
	std::vector<IDBTR> idbRules;
	std::vector<IDBValue> idbValues;

	std::set<DatalogProgram> Gs;
	std::set<DatalogProgram> Ss;




	std::vector<ConcreteRule> getConcreteRules(const DatalogProgram&);
	std::vector< std::vector<int> > execute(const DatalogProgram&);


	DPManager(const Matching& match) : M(match) {}

	void init_helper(bool general);
	void initGS();

	void exploreCandidateRules();
	void fillIDBValues(); // shoudl be called after exploreCandidateRules

	std::set<DatalogProgram> refineProg(const DatalogProgram& prog, bool specialize) const;

	int findIDBIndex(Relation r) const;

};

} // namespace SpeedyFOIL

#endif
