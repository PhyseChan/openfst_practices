#include <iostream>
#include <fst/fstlib.h>


template<class I>
void GetSymbols(const fst::SymbolTable &symtab,
                bool include_eps,
                std::vector<I> *syms_out) {
  assert(syms_out != NULL);
  syms_out->clear();
  for (fst::SymbolTableIterator iter(symtab);!iter.Done();iter.Next()) {
    if (include_eps || iter.Value() != 0) {
      syms_out->push_back(iter.Value());
      assert(syms_out->back() == iter.Value());  // an integer-range thing.
    }
  }
}

// see fstext-utils.h for comment.
template<class Arc>
void ConvertNbestToVector(const fst::Fst<Arc> &fst,
                          std::vector<fst::VectorFst<Arc> > *fsts_out) {
  typedef typename Arc::Weight Weight;
  typedef typename Arc::StateId StateId;
  fsts_out->clear();
  StateId start_state = fst.Start();
  if (start_state == fst::kNoStateId) return; // No output.
  size_t n_arcs = fst.NumArcs(start_state);
  bool start_is_final = (fst.Final(start_state) != Weight::Zero());
  fsts_out->reserve(n_arcs + (start_is_final ? 1 : 0));

  if (start_is_final) {
    fsts_out->resize(fsts_out->size() + 1);
    StateId start_state_out = fsts_out->back().AddState();
    fsts_out->back().SetFinal(start_state_out, fst.Final(start_state));
  }

  for (fst::ArcIterator<fst::Fst<Arc> > start_aiter(fst, start_state);
       !start_aiter.Done();
       start_aiter.Next()) {
    fsts_out->resize(fsts_out->size() + 1);
    fst::VectorFst<Arc> &ofst = fsts_out->back();
    const Arc &first_arc = start_aiter.Value();
    StateId cur_state = start_state,
        cur_ostate = ofst.AddState();
    ofst.SetStart(cur_ostate);
    StateId next_ostate = ofst.AddState();
    ofst.AddArc(cur_ostate, Arc(first_arc.ilabel, first_arc.olabel,
                                first_arc.weight, next_ostate));
    cur_state = first_arc.nextstate;
    cur_ostate = next_ostate;
    while (1) {
      size_t this_n_arcs = fst.NumArcs(cur_state);
      assert(this_n_arcs <= 1); // or it violates our assumptions
                                      // about the input.
      if (this_n_arcs == 1) {
        assert(fst.Final(cur_state) == Weight::Zero());
        // or problem with ShortestPath.
        fst::ArcIterator<fst::Fst<Arc>> aiter(fst, cur_state);
        const Arc &arc = aiter.Value();
        next_ostate = ofst.AddState();
        ofst.AddArc(cur_ostate, Arc(arc.ilabel, arc.olabel,
                                    arc.weight, next_ostate));
        cur_state = arc.nextstate;
        cur_ostate = next_ostate;
      } else {
        assert(fst.Final(cur_state) != Weight::Zero());
        // or problem with ShortestPath.
        ofst.SetFinal(cur_ostate, fst.Final(cur_state));
        break;
      }
    }
  }
}

template<class Arc, class I>
bool GetLinearSymbolSequence(const fst::Fst<Arc> &fst,
                             std::vector<I> *isymbols_out,
                             std::vector<I> *osymbols_out,
                             fst::SymbolTable *char_tokens,
                              fst::SymbolTable *word_tokens,
                             typename Arc::Weight *tot_weight_out) {
  typedef typename Arc::StateId StateId;
  typedef typename Arc::Weight Weight;

  Weight tot_weight = Weight::One();
  std::vector<I> ilabel_seq;
  std::vector<I> olabel_seq;

  StateId cur_state = fst.Start();
  if (cur_state == fst::kNoStateId) {  // empty sequence.
    if (isymbols_out != NULL) isymbols_out->clear();
    if (osymbols_out != NULL) osymbols_out->clear();
    if (tot_weight_out != NULL) *tot_weight_out = Weight::Zero();
    return true;
  }
  while (1) {
    Weight w = fst.Final(cur_state);
    if (w != Weight::Zero()) {  // is final..
      tot_weight = Times(tot_weight, w);
      if (fst.NumArcs(cur_state) != 0) return false;
      if (isymbols_out != NULL) *isymbols_out = ilabel_seq;
      if (osymbols_out != NULL) *osymbols_out = olabel_seq;
      if (tot_weight_out != NULL) *tot_weight_out = tot_weight;
      return true;
    } else {
      if (fst.NumArcs(cur_state) != 1) return false;

      fst::ArcIterator<fst::Fst<Arc> > iter(fst, cur_state);  // get the only arc.
      const Arc &arc = iter.Value();
      tot_weight = Times(tot_weight, arc.weight);
      if (arc.ilabel != 0) ilabel_seq.push_back(char_tokens->Find(arc.ilabel));
      if (arc.olabel != 0) olabel_seq.push_back(word_tokens->Find(arc.olabel));
      cur_state = arc.nextstate;
    }
  }
}

int main() {
    //fst::StdVectorFst *fst = fst::StdVectorFst::Read("/home/lqb/program/openfst-1.8.1/exercises/e3/fsts/res.fst");
    fst::StdVectorFst *fst = fst::StdVectorFst::Read("/home/lqb/program/openfst-1.8.1/exercises/tokenizer/fsts/best_path.fst");
    fst::SymbolTable *char_tokens = fst::SymbolTable::ReadText("/home/lqb/program/openfst-1.8.1/exercises/tokenizer/char.txt");
    fst::SymbolTable *word_tokens = fst::SymbolTable::ReadText("/home/lqb/program/openfst-1.8.1/exercises/tokenizer/mcd.txt");
    std::vector<std::string> *out_seq = new std::vector<std::string>();
    std::vector<std::string> *in = new std::vector<std::string>();
    fst::StdArc::Weight *weight = new fst::StdArc::Weight();
    std::vector<fst::VectorFst<fst::StdArc>> *nbest_path_fst = new std::vector<fst::VectorFst<fst::StdArc>>();
    ConvertNbestToVector(*fst, nbest_path_fst);
    for (int i = 0; i < nbest_path_fst->size(); i++) {
        GetLinearSymbolSequence((*nbest_path_fst)[i], in, out_seq, char_tokens, word_tokens, weight);
        for (int j = 0; j < out_seq->size(); j++) {
            std::cout << (*out_seq)[j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}