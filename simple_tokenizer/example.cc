#include <iostream>
#include <fst/fstlib.h>
#include <string>
#include <vector>

// TODO: refector the code to make the code more readable and make good use of 
// polymorphism mechanism to structure the code in a more object-oriented way.
// 重构代码。


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

// split the string by the delimiter.
// 对组合切片
std::vector<std::string>* readCharFromComb(std::string s){
    std::string delimiter = "+";
    std::vector<std::string> *str_list = new std::vector<std::string>(); 
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        str_list->push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    str_list->push_back(s);
    return str_list;
}

// TODO: figure out a better way to return the Table, which doesn't copy the object. 
// Find out a way to load the Table from a file instead of hardcoding it.
// 返回对象的时候会复制一个对象，浪费性能。 
// 查下有哪些读取从文件加载映射表的方法

// This the table that stores the mapping between the chinese character and the state.
// 汉字-id 映射表
fst::SymbolTable init_char_tokens(){
    fst::SymbolTable isym;
    isym.AddSymbol("<eps>", 0);
    isym.AddSymbol("南", 1);
    isym.AddSymbol("京", 2);
    isym.AddSymbol("市", 3);
    isym.AddSymbol("长", 4);
    isym.AddSymbol("江", 5);
    isym.AddSymbol("大", 6);
    isym.AddSymbol("桥", 7);
    isym.AddSymbol("很", 8);
    isym.AddSymbol("好", 9);
    isym.AddSymbol("看", 10);
    isym.WriteText("char_tokens.txt");
    return isym;
}

// This the table that stores the mapping between the chinese word and the state.
// 词-id 映射表
fst::SymbolTable init_word_tokens(){
    fst::SymbolTable isym;
    isym.AddSymbol("<eps>", 0);
    isym.AddSymbol("南京", 1);
    isym.AddSymbol("南京市", 2);
    isym.AddSymbol("市长", 3);
    isym.AddSymbol("长江", 4);
    isym.AddSymbol("大桥", 5);
    isym.AddSymbol("江大桥", 6);
    isym.AddSymbol("很好", 7);
    isym.AddSymbol("好看", 8);
    isym.AddSymbol("很", 9);
    isym.AddSymbol("看", 10);
    isym.AddSymbol("长江大桥", 11);
    isym.AddSymbol("南京市长", 12);
    return isym;
}

// This the table that stores the mapping between the chinese word combination and the state.
// 搭配-id 映射表
fst::SymbolTable init_comb(){
    fst::SymbolTable isym;
    isym.AddSymbol("<eps>", 0);
    isym.AddSymbol("南京+市长", 1);
    isym.AddSymbol("南京市+长江大桥", 2);
    isym.AddSymbol("市长+江大桥", 3);
    isym.AddSymbol("长江+大桥", 4);
    isym.AddSymbol("南京市长+江大桥+很+好看", 5);
    isym.AddSymbol("很+好看", 6);
    isym.AddSymbol("大桥+好看", 7);
    isym.AddSymbol("大桥+很", 8);
    isym.AddSymbol("江大桥+很", 9);
    isym.AddSymbol("长江大桥+很", 10);
    // isym.AddSymbol("南京", 6);
    // isym.AddSymbol("南京市", 7);
    // isym.AddSymbol("市长", 8);
    // isym.AddSymbol("长江", 9);
    // isym.AddSymbol("大桥", 10);
    // isym.AddSymbol("江大桥", 11);
    // isym.AddSymbol("很好", 12);
    // isym.AddSymbol("好看", 13);
    // isym.AddSymbol("很", 14);
    // isym.AddSymbol("看", 15);
    // isym.AddSymbol("长江大桥", 16);
    // isym.AddSymbol("南京市长", 17);
    return isym;
}

// This the table that stores the mapping between the word-class and the state.
// 词性-id 映射表
fst::SymbolTable init_word_class(){
    fst::SymbolTable isym;
    isym.AddSymbol("<eps>", 0);
    isym.AddSymbol("Noun", 1);
    isym.AddSymbol("Verb", 2);
    isym.AddSymbol("Adj", 3);
    isym.AddSymbol("Adv", 4);
    return isym;
}

// This funciton creates a fst for the tokenization of a sentence.
// 分词fst
fst::StdVectorFst tokenizationFst(fst::SymbolTable &char_tokens, fst::SymbolTable &word_tokens){
    fst::StdVectorFst fst;
    fst.AddState();
    fst.SetStart(0);
    fst.SetFinal(0, 0.0);
    int num_states = 0;
    
    for(int i = 1; i < word_tokens.NumSymbols(); i++){
        std::string out_label = word_tokens.Find(i);
        std::vector<std::string> *char_list = readChar(out_label);
        int out_id = word_tokens.Find(out_label);
        for (int j = 0; j < char_list->size(); j++){
            std::string in_label = char_list->at(j);
            int in_id = char_tokens.Find(in_label);
            if (j==0){
                fst.AddState();
                ++num_states;
                fst.AddArc(j, fst::StdArc(in_id, out_id, 1.0, num_states));
            }
            else{
                fst.AddState();
                ++num_states;
                fst.AddArc(num_states-1, fst::StdArc(in_id, word_tokens.Find("<eps>"), 1.0, num_states));
            }
        }
        fst.AddArc(num_states, fst::StdArc(char_tokens.Find("<eps>"), word_tokens.Find("<eps>"), 1.0, 0));
    }
    return fst;
}

// This funciton creates a fst for the combination of words.
fst::StdVectorFst wordCombiantionFst(fst::SymbolTable &word_tokens, fst::SymbolTable &comb){
    fst::StdVectorFst fst;
    fst.AddState();
    fst.SetStart(0);
    fst.SetFinal(0, 0.0);
    int num_states = 0;
    
    for(int i = 1; i < comb.NumSymbols(); i++){
        std::string out_label = comb.Find(i);
        std::vector<std::string> *word_list = readCharFromComb(out_label);
        int out_id = comb.Find(out_label);
        for (int j = 0; j < word_list->size(); j++){
            std::string in_label = word_list->at(j);
            int in_id = word_tokens.Find(in_label);
            if (j==0){
                fst.AddState();
                ++num_states;
                fst.AddArc(j, fst::StdArc(in_id, out_id, 1.0, num_states));
            }
            else{
                fst.AddState();
                ++num_states;
                fst.AddArc(num_states-1, fst::StdArc(in_id, word_tokens.Find("<eps>"), 1.0, num_states));
            }
        }
        fst.AddArc(num_states, fst::StdArc(word_tokens.Find("<eps>"), word_tokens.Find("<eps>"), 1.0, 0));
    }
    return fst;
}

// This funciton is used to compose two fsts.
// 合并fst
fst::StdVectorFst composeTokenAndCombFst(fst::StdVectorFst &tokenizationFst, fst::StdVectorFst &combination_fst){
    fst::StdVectorFst fst;
    fst::Compose(tokenizationFst, combination_fst, &fst);
    return fst;
}

// This funciton creates a fst from a sentence.
fst::StdVectorFst createInputFst(std::string input, fst::SymbolTable &char_tokens){
    std::vector<std::string>* input_chars = readChar(input);
    fst::StdVectorFst ifst;
    ifst.AddState();
    ifst.SetStart(0);
    int num_states = 0;
    for (int i = 0; i < input_chars->size(); i++){
        ifst.AddArc(num_states, fst::StdArc(char_tokens.Find(input_chars->at(i)), char_tokens.Find(input_chars->at(i)), 1.0, num_states+1));
        num_states++;
        ifst.AddState();
    }
    ifst.SetFinal(num_states, 0.0);
    return ifst;
}

int main(){
    fst::SymbolTable char_tokens = init_char_tokens();
    fst::SymbolTable word_tokens = init_word_tokens();
    fst::SymbolTable comb = init_comb();
    fst::StdVectorFst fst = tokenizationFst(char_tokens, word_tokens);
    fst::StdVectorFst fst2 = wordCombiantionFst(word_tokens, comb);
    fst.SetInputSymbols(&char_tokens);
    fst.SetOutputSymbols(&word_tokens);
    fst.Write("fsts/tokenization.fst");

    fst2.SetInputSymbols(&word_tokens);
    fst2.SetOutputSymbols(&comb);
    fst2.Write("fsts/word_combination.fst");

    fst::StdVectorFst composed_tokenizer_combination_fst = composeTokenAndCombFst(fst, fst2);
    fst::Determinize(composed_tokenizer_combination_fst, &composed_tokenizer_combination_fst);
    fst::Minimize(&composed_tokenizer_combination_fst);
    composed_tokenizer_combination_fst.Write("fsts/tokenization_and_combination.fst");

    std::string input = "南京市长江大桥很好看";
    fst::StdVectorFst input_fst = createInputFst(input, char_tokens);
    input_fst.SetInputSymbols(&char_tokens);

    // parse the sentence 
    // 解析句子
    fst::StdVectorFst result;
    fst::Compose(input_fst, composed_tokenizer_combination_fst, &result);
    fst::RmEpsilon(&result);
    // fst::ShortestPath(result, &result);
    result.Write("fsts/result.fst");

    return 0;
}