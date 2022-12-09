#include <iostream>
#include <fst/fstlib.h>
#include <string>

fst::SymbolTable init_isym(){
    fst::SymbolTable isym;
    isym.AddSymbol("<eps>", 0);
    isym.AddSymbol("1", 1);
    isym.AddSymbol("2", 2);
    isym.AddSymbol("3", 3);
    isym.AddSymbol("4", 4);
    isym.AddSymbol("5", 5);
    isym.AddSymbol("6", 6);
    isym.AddSymbol("7", 7);
    isym.AddSymbol("8", 8);
    isym.AddSymbol("9", 9);
    isym.AddSymbol("0", 10);
    return isym;
}

fst::SymbolTable init_osym(){
    fst::SymbolTable osym;
    osym.AddSymbol("<eps>", 0);
    osym.AddSymbol("one", 1);
    osym.AddSymbol("two", 2);
    osym.AddSymbol("three", 3);
    osym.AddSymbol("four", 4);
    osym.AddSymbol("five", 5);
    osym.AddSymbol("six", 6);
    osym.AddSymbol("seven", 7);
    osym.AddSymbol("eight", 8);
    osym.AddSymbol("nine", 9);
    osym.AddSymbol("ten", 10);
    osym.AddSymbol("eleven", 11);
    osym.AddSymbol("twelve", 12);
    osym.AddSymbol("thirteen", 13);
    osym.AddSymbol("fourteen", 14);
    osym.AddSymbol("fifteen", 15);
    osym.AddSymbol("sixteen", 16);
    osym.AddSymbol("seventeen", 17);
    osym.AddSymbol("eighteen", 18);
    osym.AddSymbol("nineteen", 19);
    osym.AddSymbol("twenty", 20);
    osym.AddSymbol("thirty", 21);
    osym.AddSymbol("forty", 22);
    osym.AddSymbol("fifty", 23);
    osym.AddSymbol("sixty", 24);
    osym.AddSymbol("seventy", 25);
    osym.AddSymbol("eighty", 26);
    osym.AddSymbol("ninety", 27);
    osym.AddSymbol("hundred", 28);
    osym.AddSymbol("thousand", 29);
    return osym;
}

fst::StdVectorFst create_fst(fst::SymbolTable &isym, fst::SymbolTable &osym){
    fst::StdVectorFst fst_obj;
    fst_obj.SetInputSymbols(&isym);
    fst_obj.SetOutputSymbols(&osym);

    fst_obj.AddState();
    fst_obj.SetStart(0);
    // create arcs for thousand place
    fst_obj.AddArc(0, fst::StdArc(10, 0, 1, 1));
    for(int i=2; i<10; i++){
        fst_obj.AddArc(0, fst::StdArc(i, 18+i, 1, 2));
    }
    fst_obj.AddArc(0, fst::StdArc(1, 0, 1, 3));

    fst_obj.AddStates(3); //states 1-3

    for(int i=1; i<10; i++){
        fst_obj.AddArc(1, fst::StdArc(i, i, 1, 5));
    }

    fst_obj.AddArc(2, fst::StdArc(10, 0, 1, 6));
    for(int i=1; i<10; i++){
        fst_obj.AddArc(2, fst::StdArc(i, i, 1, 6));
    }
    fst_obj.AddArc(3, fst::StdArc(10, 10, 1, 7));
    for (int i=1; i<10; i++){
        fst_obj.AddArc(3, fst::StdArc(i, i+10, 1, 7));
    }

    fst_obj.AddStates(4); //states 4-7

    for (int i=5; i<8; i++){
        fst_obj.AddArc(i, fst::StdArc(0, 29, 1, 8));
    }
    fst_obj.AddArc(1, fst::StdArc(10, 0, 1, 8));

    fst_obj.AddState(); // states 8

    // create arcs for hundred place
    fst_obj.AddStates(2);
    fst_obj.AddArc(8, fst::StdArc(10, 0, 1, 10));
    for(int i=1; i<10; i++){
        fst_obj.AddArc(8, fst::StdArc(i, i, 1, 9));
    }
    
    fst_obj.AddArc(9, fst::StdArc(0, 28, 1, 10));
    
    // create arcs for ten place
    fst_obj.AddArc(10, fst::StdArc(10, 0, 1, 11));
    fst_obj.AddArc(10, fst::StdArc(1, 0, 1, 12));
    for (int i = 2; i < 10; i++)
    {
        fst_obj.AddArc(10, fst::StdArc(i, 18+i, 1, 13));
    }
    
    fst_obj.AddStates(3); // states 13-15

    // create arcs for one place
    fst_obj.AddArc(13, fst::StdArc(10, 0, 1, 14));
    for (int i = 1; i < 10; i++)
    {
        fst_obj.AddArc(13, fst::StdArc(i, i, 1, 14));
    }
    for (int i=0; i<10; i++){
        fst_obj.AddArc(12, fst::StdArc(i, i+10, 1, 14));
    }
    fst_obj.AddArc(11, fst::StdArc(10, 0, 1, 14));
    for (int i=1; i<10; i++){
        fst_obj.AddArc(11, fst::StdArc(i, i, 1, 14));
    }
    fst_obj.AddState(); // state 16

    fst_obj.SetFinal(14, 0);

    return fst_obj;
}

fst::StdVectorFst convertDigit(int digit[], fst::SymbolTable &isym, fst::StdVectorFst &digit_graph){
    fst::StdVectorFst fst_obj;
    fst::StdVectorFst fst_out;
    fst_obj.SetInputSymbols(&isym);
    fst_obj.AddStates(6);
    fst_obj.SetStart(0);
    fst_obj.SetFinal(5);
    for (int i=0; i<5; i++){
        int n = digit[i];
        if (digit[i] == 0){
            n = 10;
        }
        fst_obj.AddArc(i, fst::StdArc(n, n, 1, i+1));
    }
    fst::Compose(fst_obj, digit_graph, &fst_out);
    return fst_out;
}

fst::StdVectorFst detAndMin(fst::StdVectorFst &fst_obj){
    fst::StdVectorFst fst_out;
    fst::Determinize(fst_obj, &fst_out);
    fst_out.Write("fsts/det.fst");
    fst::Minimize(&fst_out);
    fst_out.Write("fsts/min.fst");
    return fst_out;
}

int main(){
    fst::SymbolTable isym = init_isym();
    fst::SymbolTable osym = init_osym();
    fst::StdVectorFst fst_obj = create_fst(isym, osym);
    fst_obj.Write("fsts/digit.fst");
    fst_obj = detAndMin(fst_obj);
    std::string s;
    std::cout << "input a 5 digits number: " << std::endl;
    std::cin >> s;
    int digits[5];
    for (int i=0; i<5; i++){
        digits[i] = s[i] - '0';
    }
    fst::StdVectorFst res=convertDigit(digits, isym, fst_obj);
    res.Write("fsts/res.fst");

    return 0;
}