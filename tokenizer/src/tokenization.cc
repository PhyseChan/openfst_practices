#include <iostream>
#include <fst/fstlib.h>
// TODO: evaluate if this method is efficient.
// 网上复制的中文字符切片代码，不知道效率如何。
std::vector<std::string>* readChar(std::string text){
    std::vector<std::string> *str_list = new std::vector<std::string>(); 
    for(size_t i = 0; i < text.length();)
    {
        int cplen = 1;
        if((text[i] & 0xf8) == 0xf0) cplen = 4;
        else if((text[i] & 0xf0) == 0xe0) cplen = 3;
        else if((text[i] & 0xe0) == 0xc0) cplen = 2;
        if((i + cplen) > text.length()) cplen = 1;

        str_list->push_back(text.substr(i, cplen));
        i += cplen;
    }
    return str_list;
}
// This funciton creates a fst from a sentence.
fst::StdVectorFst createInputFst(std::string input, fst::SymbolTable *char_tokens){
    std::vector<std::string>* input_chars = readChar(input);
    fst::StdVectorFst ifst;
    ifst.AddState();
    ifst.SetStart(0);
    int num_states = 0;
    for (int i = 0; i < input_chars->size(); i++){
        ifst.AddArc(num_states, fst::StdArc(char_tokens->Find(input_chars->at(i)), char_tokens->Find(input_chars->at(i)), 1.0, num_states+1));
        num_states++;
        ifst.AddState();
    }
    ifst.SetFinal(num_states, 0.0);
    return ifst;
}
int main(int argc, char *argv[]) {
    std::string input = argv[1];
    fst::SymbolTable *char_tokens = fst::SymbolTable::ReadText("char.txt");
    fst::SymbolTable *word_tokens = fst::SymbolTable::ReadText("mcd.txt");
    fst::StdVectorFst *token_fst = fst::StdVectorFst::Read("fsts/tokenization.fst");

    fst::StdVectorFst input_fst = createInputFst(input, char_tokens);
    input_fst.SetInputSymbols(char_tokens);
    fst::StdVectorFst result;
    fst::Compose(input_fst, *token_fst, &result);
    result.Write("fsts/result.fst");
    fst::StdVectorFst best_path;
    fst::ShortestPath(result, &best_path, 5);
    fst::RmEpsilon(&best_path);
    best_path.Write("fsts/best_path.fst");
    return 0;
}