//
// Created by finne on 5/31/2020.
//

#include "uci.h"


Board *board;
std::thread *searchThread = nullptr;


void uci_loop(){

    bb_init();
    search_init(128);
//    perft_init(false);
    
    
    std::wcout << "Koivisto 64 bit by K. Kahre, F. Eggers" << std::endl;

    board = new Board();
    
    
    std::atexit(uci_quit);
    std::string line;
    
//    uci_processCommand("position startpos moves d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8b7 f1g2 b8c6 b1c3 f8b4 e1g1 d7d5 c4d5 e6d5 a2a3 b4c3 b2c3 h7h6 f1e1 e8g8 f3e5 d8e8 f2f4 c6e7 a3a4 f6e4 g2e4");
//    uci_processCommand("go depth 8");
    
    while (true) {
        getline(cin, line);
        
        if (line == "quit") {
            exit(0);
        } else {
            uci_processCommand(line);
        }
    }
    
}

void uci_uci() {
    std::cout << "id name Koivisto" << std::endl;
    std::cout << "id author K. Kahre, F. Eggers" << std::endl;
    std::cout << "option name Hash type spin default 128 min 1 max 4096" <<std::endl;
    
    
    std::cout << "uciok" << std::endl;
}

std::string uci_getValue(std::vector<std::string> &vec, std::string key){
    int index = 0;
    for(std::string s:vec){
        if(s == key){
            return vec.at(index+1);
        }
        index ++;
    }
    return "";
}



void uci_endThread(){
    if(searchThread != nullptr){
        delete searchThread;
        searchThread = nullptr;
    }
}

void uci_searchAndPrint(Depth maxDepth, int maxTime){
    Move m = bestMove(board, maxDepth, maxTime);
    std::cout << "bestmove " << toString(m) << std::endl;
    uci_endThread();
}

void uci_processCommand(std::string str) {
    
    str = trim(str);
    
    std::vector<std::string> split{};
    splitString(str, split, ' ');

//    std::cout << str << std::endl;
    
//    std::cout << "...";
//
//    for(string s:split){
//        std::cout << s << std::endl;
//    }
//    std::cout << "...";
    
    if (split.at(0) == "uci"){
        uci_uci();
    }else if (split.at(0) == "setoption") {
        
        string name = uci_getValue(split, "name");
        string value = uci_getValue(split, "value");
        
        std::cout<< "-" << name << "-  -"<<value<<"-"<<std::endl;
        
        uci_set_option(name, value);
    }else if (split.at(0) == "go"){
        if(str.find("wtime") != string::npos){
            string wtime = uci_getValue(split, "wtime");
            string btime = uci_getValue(split, "btime");
            string wincr = uci_getValue(split, "winc");
            string bincr = uci_getValue(split, "binc");
            string mvtog = uci_getValue(split, "movestogo");
            
            uci_go_match(
                    (wtime.empty()) ? 60000000:stoi(wtime),
                    (btime.empty()) ? 60000000:stoi(btime),
                    (wincr.empty()) ? 0:stoi(wincr),
                    (bincr.empty()) ? 0:stoi(bincr),
                    (mvtog.empty()) ? 40:stoi(mvtog));
        
        }else if(str.find("depth") != string::npos){
            uci_go_depth(stoi(uci_getValue(split, "depth")));
        }else if(str.find("nodes") != string::npos){
            uci_go_nodes(stoi(uci_getValue(split, "nodes")));
        }else if(str.find("movetime") != string::npos){
            uci_go_time(stoi(uci_getValue(split, "movetime")));
        }else if(str.find("infinite") != string::npos){
            uci_go_infinite();
        }else if(str.find("mate") != string::npos){
            uci_go_mate(stoi(uci_getValue(split, "mate")));
        }
    }else if(split.at(0) == "stop"){
        uci_stop();
    }else if(split.at(0) == "isready"){
        uci_isReady();
    }else if(split.at(0) == "debug"){
        uci_debug(uci_getValue(split, "debug") == "on");
    }else if(split.at(0) == "position"){
    
        auto fenPos = str.find("fen");
        auto movePos = str.find("moves");
        
        string moves{};
        if(movePos != string::npos){
            moves = str.substr(movePos+5);
            moves = trim(moves);
        }
        
        if(fenPos != string::npos){
            string fen = str.substr(fenPos+3, movePos);
            fen = trim(fen);
            uci_position_fen(fen, moves);
        }else{
            uci_position_startpos(moves);
        }
    }else if(split.at(0) == "print"){
        std::cout << *board << std::endl;
    }
}

void uci_go_match(int wtime, int btime, int winc, int binc, int movesToGo) {
    
    if(searchThread != nullptr){
        return;
    }
    
    int timeToUse = board->getActivePlayer() == WHITE ? (wtime/40 + winc) : (btime/40 + binc);
    std::cout << timeToUse << std::endl;
    
    searchThread = new std::thread(uci_searchAndPrint, MAX_PLY, timeToUse);
    searchThread->detach();
    
}

void uci_go_depth(int depth) {
    
    if(searchThread != nullptr){
        return;
    }
    
    searchThread = new std::thread(uci_searchAndPrint,  depth, ONE << 30);
    searchThread->detach();
}

void uci_go_nodes(int nodes) {

}

void uci_go_time(int movetime) {

}

void uci_go_infinite() {

}

void uci_go_mate(int depth) {

}

void uci_stop() {
    search_stop();
}

void uci_set_option(std::string name, std::string value) {
    if(name == "hash") search_setHashSize(stoi(value));
}

void uci_isReady() {
    //TODO check if its running
    
    std::cout << "readyok" << std::endl;
}

void uci_debug(bool mode) {

}

void uci_position_fen(std::string fen, std::string moves) {
    if(board != nullptr) delete board;
    board = new Board{fen};
    
//    std::cout << "-" << fen << "-" << std::endl;
//    std::cout << "-" << moves << "-" << std::endl;
    
    
    if(moves.empty()) return;
    std::vector<string> mv{};
    splitString(moves, mv, ' ');
    
    for(string s:mv){
        
        string str1 = s.substr(0,2);
        string str2 = s.substr(2,4);
        Square s1 = squareIndex(str1);
        Square s2 = squareIndex(str2);
        
        Piece moving = board->getPiece(s1);
        Piece captured = board->getPiece(s2);
        
        assert(moving >= 0);
        
        Type type;
        
        if(s.size() > 4){
            
            Piece promo;
            switch(s.at(4)){
                case 'q': promo = QUEEN; break;
                case 'r': promo = ROOK; break;
                case 'b': promo = BISHOP; break;
                case 'n': promo = KNIGHT; break;
            }
            
            if(captured >= 0){
                type = 11 + promo;
            }else{
                type = 7 + promo;
            }
            
        }else{
            if((moving%6) == KING){
                if(abs(s2-s1) == 2){
                    if(s2 > s1){
                        type = KING_CASTLE;
                    }else{
                        type = QUEEN_CASTLE;
                    }
                }else{
                    if(captured >= 0){
                        type = CAPTURE;
                    }else{
                        type = QUIET;
                    }
                }
            }else if((moving % 6) == PAWN){
                if(abs(s2-s1) == 16){
                    type = DOUBLED_PAWN_PUSH;
                }else if(abs(s2-s1) != 8){
                    if(captured >= 0){
                        type = CAPTURE;
                    }else{
                        type = EN_PASSANT;
                    }
                }else{
                    type = QUIET;
                }
            }else{
                if(captured >= 0){
                    type = CAPTURE;
                }else{
                    type = QUIET;
                }
            }
        }
        Move m = genMove(s1,s2,type, moving, captured);
        
        assert(board->isLegal(m));
        board->move(m);
    
    }
    
}

void uci_position_startpos(std::string moves) {
    uci_position_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", moves);
}

void uci_quit() {
    
    delete board;
    board = nullptr;
    
    
    bb_cleanUp();
    search_cleanUp();
    
//    perft_cleanUp();
}