    #include "ID.h"
    #include "symOb.h"
    #include "LitOb.h"
    #include <map>
    #include<string>
    #include <sstream>
    #include <stdio.h>
    #include <windows.h>
    #include <vector>
    #include <exception>
    #include <stack>
    #include<queue>
    #include <cctype>
    #include <iomanip>
    using namespace std;

    map <string,ID*>opTab;
    map <string,symOb*>symTab;
    vector<string>modified;
    queue<string>litTab;
    map <string,LitOb*>litPool;
    int locctr=-1;
    int programlength=0;
    int start=0;
    int lineNum = 1;
    int eFile = 0;
    int length=-1;
    bool okTwo=true;
    FILE* infile;
    FILE* outfile;
    FILE* readFile;
    FILE* writeFile;
    FILE* objFile;
    string current;
    string base="";
    string currentLoc="";
    string lineObject="";
    string startLabel="";
    string starttloc="";
    string lineStartLoc="";
    string bits="";
    bool ok =true;
    bool absolute = false;

    string fgetLine(FILE* file);
    string formatModify( int loc , string s [] );
    string hexaChange( long s);
    int evaluatePostFix(string postfix);
    void passOne();
    void getParsedLine(string out[]);
    vector<string> split(string str1,string sf);
    string infixToPostfix(vector<string> str);
    void secondaryParsing (string Field [] , int splitSize);
    int primaryParsing(string inputLine, string * line);
    string wordObj(string str);
    string byteObj( string str );
    bool inHashMap(string opcode);
    bool inSymTab(string symbol);
    int getAddress(string label);
    string uppercaseString(string s);
    void print(FILE* pFile,string s);
    string toString(int i);
    string commentFormat(string comment);
    string errorFormat(string error);
    string format( int line , int loc , string s [] );
    unsigned int hexToInt(string hex);
    void init();
    bool checkWord(string word);
    int checkByte(string byte);
    int checkReserve(string res);
    string firstRow();
    string rowTableFormat(string name,int address , int r);
    void printSymTab();
    int initFiles(int argc, char *argv[]);
    int stringToInteger(string number);
    int modify(string line []);
    int evaluatePostFix(string postfix);
    void initPassTwo();
    string pccounter(string operand,string next,string a[]);
    void pass2Processing ();
    string textLine();
    string header();
    string ends();
    void checkRegisterOperand ( string operation , string operand , int output []);
    string formatObjectCode (int opcode , string bits , string bp , string disp , int format , int reg1 , int reg2);
    void checkTwoRegister ( string operand ,bool shiftOperation ,int output []);
    void checkOneRegister (string operand ,bool svc, int output [] );
    int findRegisterNumber( char reg );
    int convertBitsToInt (string bits);
    char intToHexChar(string str);
    void Map();
    bool isLiteral(string line);
    bool isByte(string line);

    int main(int argc, char *argv[] )
    {
        try
        {
            if ( initFiles(argc,argv) == 0)
            {
                return 0;
            }
            init();
            Map();
            passOne();
            cout << "   Pass1 Complete !!\n\n";
            if ( ok )
            {
                print(outfile,">>  e n d    o f   p a s s   1 ");
                print(outfile,">>  *****************************************************************************************");
                print(outfile,">>  *****************************************************************************************");
            }
            else
            {
                print(outfile,">>  Incomplete   p a s s   1 ");
                print(outfile,">>  *****************************************************************************************");
                print(outfile,">>  *****************************************************************************************");
            }
            print(outfile,">>  s y m b o l     t a b l e   (values in Hexa.)");
            print(outfile,firstRow());
            print(outfile,"     -----------------------------------");
            printSymTab();
            fclose(infile);
            fclose(outfile);
            if ( ok )
            {
                initPassTwo();
                pass2Processing();
            }
            if ( !okTwo )
            {
               // objFile = fopen("OBJFILE.txt","w");
                    //remove( "OBJFILE.txt" );
                    fclose(objFile);
                   objFile = fopen("OBJFILE.txt","w");
            }
            fclose(writeFile);
            fclose(readFile);
            fclose(objFile);
        }
        catch(exception& e)
        {

        }
        return 0;
    }


    bool isLiteral(string line)
    {

        //cout << "inside is literal ." << line << "." << endl;

        if(line.empty())
        {
            return false;
        }

        if(line.at(0) != '=')
        {
            //cout << line << "failed =" <<endl;
            return false;
        }

        if(!(line.at(1) == 'x' || line.at(1) == 'X' || line.at(1) == 'C' || line.at(1) == 'c'))
        {
            //  cout << line << "failed x,c" <<endl;
            return false;
        }

        if(line.at(2) != '\'')
        {
            //cout << line << "failed first'" <<endl;
            return false;
        }

        if(line.at(line.length()-1) != '\'')
        {
            // cout << line << "failed last '" <<endl;
            return false;
        }
        //cout << line <<"it is a literal" << endl;
        return true;
    }

    bool isByte(string line)
    {

        //cout << "inside is byte ." << line << "." << endl;

        if(line.empty())
        {
            return false;
        }

        if(!(line.at(0) == 'x' || line.at(0) == 'X' || line.at(0) == 'C' || line.at(0) == 'c'))
        {
            //cout << line << "byte failed xc" <<endl;
            return false;
        }

        if(line.at(1) != '\'')
        {
            //cout << line << " byte failed first'" <<endl;
            return false;
        }

        if(line.at(line.length()-1) != '\'')
        {
            //  cout << line << " byte failed last'" <<endl;
            return false;
        }
        //cout << line << "IT IS a BYTE" << endl;
        return true;
    }


    char intToHexChar(string str)
    {
        char* ptr;
        long parsed = strtol(str.c_str (), & ptr , 2);
        return hexaChange(parsed).at(0);
    }

    int convertBitsToInt (string bits)
    {

        //cout<<"bits :"<<bits<<endl;
        char first = bits.at(0);
        char second = bits.at(1);
        if (first == '0' && second == 0)
        {
            return 0;
        }
        else if (first == '0' && second == '1')
        {
            return 1;
        }
        else if (first == '1' && second == '0')
        {
            return 2;
        }
        else if (first == '1' && second == '1')
        {
            return 3;
        }
        else
        {
            //cout<<"convertBitsToIntError"<<endl;
            return -1;
        }

    }

    int findRegisterNumber( char reg )
    {
        if (reg == 'A')
        {
            return 0;
        }
        else if (reg == 'X')
        {
            return 1;
        }
        else if (reg == 'L')
        {
            return 2;
        }
        else if (reg == 'B')
        {
            return 3;
        }
        else if (reg == 'S')
        {
            return 4;
        }
        else if (reg == 'T')
        {
            return 5;
        }
        else if (reg == 'F')
        {
            return  6;
        }
        else
        {
            //cout<<"Register Error"<<endl;
            return -1;
        }
    }

    void checkOneRegister (string operand ,bool svc, int output [])
    {
        string operandTrue = "";

        for (int i = 0; i < operand.length(); i++)
        {
            if (!(operand.at(i)==' '))
            {
                operandTrue += operand.at(i);
            }
        }

        if (!(operandTrue.length() == 1))
        {
            output[0] = 0;
            output[1] =-1;
            output[2] =-1;
        }
        else
        {
            int registerNumber = -1;

            if (svc == true)
            {
                registerNumber = operandTrue.at(0)-48;
                registerNumber = registerNumber - 1 ;
            }
            else
            {
                registerNumber = findRegisterNumber(operandTrue.at(0) ) ;

            }

            if (!(registerNumber >=0 && registerNumber <= 6))
            {
                output[0] = 0;
                output[1] =-1;
                output[2] =-1;

            }
            else
            {
                output[0] = 1;
                output[1] = registerNumber;
                output[2] =0;

            }


        }
    }

    void checkTwoRegister ( string operand ,bool shiftOperation ,int output [])
    {

        string operandTrue = "";

        for (int i = 0; i < operand.length(); i++)
        {
            if (!(operand.at(i)==' ' || operand.at(i)==','))
            {
                operandTrue += operand.at(i);
            }
        }

        //cout<< "operand true : "<<operandTrue<<endl;

        if (!(operandTrue.length() == 2))
        {
            //cout<<"error 2 registers"<<endl;
            output[0] = 0;
            output[1] =-1;
            output[2] =-1;
        }
        else
        {

            //  int registerNumber1 = -1;
            //  int registerNumber2 = -1;

            int registerNumber1 = findRegisterNumber(operandTrue.at(0));
            int registerNumber2;
            if (shiftOperation == true)
            {
                registerNumber2 = operandTrue.at(1)-48;
                registerNumber2 = registerNumber2 - 1 ;
            }
            else
            {
                registerNumber2 = findRegisterNumber(operandTrue.at(1));
            }


            if (!((registerNumber1 >=0 && registerNumber1 <= 6)||(registerNumber2 >=0 && registerNumber2 <= 6)))
            {
                output[0] = 0;
                output[1] =-1;
                output[2] =-1;

            }
            else
            {
                output[0] = 1;
                output[1] = registerNumber1;
                output[2] = registerNumber2;

            }

        }


    }

    string formatObjectCode (int opcode , string bp , string disp , int format , int reg1 , int reg2)
    {
        string objectCodeOutput = "";

        // fix the coming strings

        //  if (bits.size() <6)
        // {
        //cout << "bits size error"<<endl;
        //     int i = 0;
        //    while (i<(6-bits.size()))
        //   {
        //      bits = '0'+bits;
        //     i++;
        //   }
        // }

        // set the new b & p bits
        //cout<<"bittttts size  "<<bits.length()<<endl;
        bits.at(3) = bp.at(0);
        bits.at(4) = bp.at(1);
        //  cout<< bits<<endl;
        //cout<<bits<<endl;



        if (format == 4)
        {
            int checck=hexToInt(disp);
            if(checck<0)
            {
                if(disp.length()>5)
                {
                    disp=disp.substr(disp.length()-5,5);
                }
                else
                {
                    if (disp.size() <5 )
                    {
                        int i = 0;
                        int dispSize=disp.size();
                        while (i<(5-dispSize))
                        {
                            disp = '0'+disp;
                            i++;
                        }

                    }
                }
            }
            else
            {

                if (disp.size() <5 )
                {
                    int i = 0;
                    int dispSize=disp.size();
                    while (i<(5-dispSize))
                    {
                        disp = '0'+disp;
                        i++;
                    }

                }
            }

        }
        else if (format == 3)
        {
            int checck=hexToInt(disp);
            if(checck<0)
            {
                if(disp.length()>3)
                {
                    disp=disp.substr(disp.length()-3,3);
                }
                else
                {
                    if (disp.size() <3 )
                    {
                        int i = 0;
                        int dispSize=disp.size();
                        while (i<(3-dispSize))
                        {
                            disp = '0'+disp;
                            i++;
                        }

                    }
                }
            }
            else
            {
                if (disp.size() <3 )
                {
                    int i = 0;
                    int dispSize=disp.size();
                    while (i<(3-dispSize))
                    {
                        disp = '0'+disp;
                        i++;
                    }

                }
            }
        }
        // set the object code of format 2
        else if ( format == 2)
        {


            char register1 = reg1+48;
            char register2 = reg2+48;

            objectCodeOutput = hexaChange(opcode)+register1+register2;

        }

        // set the object code of format 1
        else if ( format == 1)
        {

            objectCodeOutput = hexaChange(opcode);

        }



        // prepare the object code with n & i for format 3 or 4
        if (format == 4 || format == 3)
        {

            int ni = convertBitsToInt (bits);
            //  cout<<"ni : "<<ni<<endl;
            string firstTwoBytes = hexaChange(ni + opcode);
            if (firstTwoBytes.size() != 2)
            {
                firstTwoBytes = '0'+firstTwoBytes;
            }
            //cout<<"firstTwoBytes : "<<firstTwoBytes<<endl;
            string xbpe = "";
            for(int i = 2 ; i < 6; i++)
            {
                xbpe += bits.at(i);
            }
            string theThirdByte = "";
            //cout<<"xbpe : "<<xbpe<<endl;
            theThirdByte += intToHexChar(xbpe);
            //cout<<"the third byte : "<<theThirdByte<<endl;
            objectCodeOutput = firstTwoBytes + theThirdByte + disp;
        }

        objectCodeOutput = uppercaseString(objectCodeOutput);

        return objectCodeOutput;
    }


    // checking format 2 registers
    void checkRegisterOperand ( string operation , string operand , int output [])
    {

        if (operation == "CLEAR" || operation == "TIXR" || operation == "SVC" ) // 1 register
        {
            bool svc = false;
            if (operation == "SVC")
            {
                svc = true;
            }
            checkOneRegister (operand , svc , output);

        }
        else
        {
            bool shiftOperation = false;
            if (operation == "SHIFTR" || operation == "SHIFTL")
            {
                //cout<<"shoft operation correct"<<endl;
                shiftOperation = true;

            }
            checkTwoRegister (operand , shiftOperation , output);

        }


    }
    void passOne()
    {
        string line[4]="";
        int splitSize=primaryParsing(fgetLine(infile),line);

    //    line[0] = uppercaseString(line[0]);
    //    line[1] = uppercaseString(line[1]);
    //    line[3] = uppercaseString(line[3]);

        if ( !(isLiteral(line[0]) || isByte(line[0])) )
        {
            //   cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[0] << endl;
            line[0] = uppercaseString(line[0]);
        }

        if ( !(isLiteral(line[1]) || isByte(line[1])) )
        {
            // cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[1] << endl;
            line[1] = uppercaseString(line[1]);
        }

        if ( !(isLiteral(line[2]) || isByte(line[2])) )
        {
            // cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[2] << endl;
            line[2] = uppercaseString(line[2]);
        }

        if ( !(isLiteral(line[3]) || isByte(line[3])) )
        {
            // cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[3] << endl;
            line[3] = uppercaseString(line[3]);
        }

        secondaryParsing(line,splitSize+1);

        string opcode = line[1];
        string operand = line[2];
        while(opcode.compare("END") != 0 && eFile!=-1 )
        {
            if ( line[0].empty() && line[1].empty() && line[2].empty() && !line[3].empty() )
            {
                print(outfile,commentFormat(line[3]));
            }
            else if(line[0].empty() && line[1].empty() && line[2].empty() &&line[3].empty()) {}
            else if(line[1].empty()) {}
            else if(opcode.compare("START") == 0&&locctr==-1)
            {
                //////newwwwwwwwwwwwwwwwwwwwwww
                startLabel=line[0];
                string temp="";
                temp=line[2];
                if(temp.empty())
                {
                    locctr=0;
                    start=0;
                    starttloc="000000";
                    currentLoc="000000";
                    lineStartLoc="000000";
                    string error="***error with start";
                    print(outfile,format(lineNum,locctr,line));
                    print(outfile,errorFormat(error));
                    ok=false;
                }
                else
                {
                    if(temp.compare("0")==0)
                    {
                        locctr=0;
                        start=0;
                        starttloc="000000";
                        currentLoc="000000";
                        lineStartLoc="000000";
                        print(outfile,format(lineNum,locctr,line));
                    }
                    else
                    {
                        int temp2 = checkReserve(temp);
                        if(temp2 ==-1)
                        {
                            locctr=0;
                            start=0;
                            starttloc="000000";
                            currentLoc="000000";
                            lineStartLoc="000000";
                            string error="***error with start";
                            print(outfile,format(lineNum,locctr,line));
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            string convert = toString(temp2);
                            locctr=hexToInt(convert);
                            start=hexToInt(convert);
                            for(int i=0; i<6-convert.length(); i++)
                            {
                                starttloc=starttloc+"0";
                            }
                            starttloc=starttloc+convert;
                            currentLoc=starttloc;
                            lineStartLoc=starttloc;
                            print(outfile,format(lineNum,locctr,line));
                        }
                    }
                }
            }
            else if(opcode.compare("START") == 0&&locctr!=-1)
            {
                string error="***error with start";
                print(outfile,format(lineNum,locctr,line));
                print(outfile,errorFormat(error));
                ok=false;
            }
            else
            {
                if ( locctr == -1 )
                {
                    locctr=0;
                    start=0;
                    starttloc="000000";
                    currentLoc="000000";
                    lineStartLoc="000000";
                }
                if(opcode.compare("EQU") == 0)
                {
                    string label=line[0];
                    operand=line[2];
                    //cout<<"operaaaaaaaaaaaaaaaaaaaaand  "<<operand<<endl;
                    if(operand.empty() || label.empty()||operand[0]=='=')
                    {
                        print(outfile,format(lineNum,locctr,line));
                        string error="*** invalid empty operand or label !!";
                        print(outfile,errorFormat(error));
                        ok=false;
                    }
                    else
                    {
                        if(operand.compare("*") == 0)
                        {
                            label=line[0];
                            symOb*symbol;
                            symbol=symTab.find(label)->second;
                            if(symbol==NULL)
                            {
                                print(outfile,format(lineNum,locctr,line));
                                symTab[label]=new symOb(locctr,true);
                            }
                            else
                            {
                                print(outfile,format(lineNum,locctr,line));
                                string error="*** duplicate symbol";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }
                        }
                        else
                        {
                            // whether it was an expression or not, call     to get the address & absolute boolean
                            // call method omar "infix to postfix"

                            string postfix = infixToPostfix(split(operand," "));
                            // call method (tarek & khaled) "evaluate"
                            absolute = false;
                            int address = evaluatePostFix(postfix);
                            if(address == -1)
                            {
                                print(outfile,format(lineNum,locctr,line));
                                string error="*** invalid operand";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }
                            string label=line[0];
                            symOb*symbol;
                            symbol=symTab.find(label)->second;
                            if(symbol==NULL)
                            {
                                print(outfile,format(lineNum,locctr,line));
                                symTab[label]=new symOb(address,!absolute);
                                //cout<< absolute<< "absolute + line 0 ???? "<< line[0] << endl;

                            }
                            else
                            {
                                print(outfile,format(lineNum,locctr,line));
                                string error="*** duplicate symbol";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }

                        }
                    }
                }
                else if(opcode.compare("ORG") == 0)
                {
                    if(operand.empty()||operand[0]=='=')
                    {
                        print(outfile,format(lineNum,locctr,line));
                        string error="*** invalid empty operand !!";
                        print(outfile,errorFormat(error));
                        ok=false;
                    }
                    else
                    {
                        // call method omar "infix to postfix"
                        // cout << operand<<endl;
                        string postfix = infixToPostfix(split(operand," "));
                        // call method (tarek & khaled) "evaluate"
                        int address = evaluatePostFix(postfix);

                        if(address == -1)
                        {
                            print(outfile,format(lineNum,locctr,line));
                            string error="*** invalid operand !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            print(outfile,format(lineNum,locctr,line));
                            locctr=address;
                        }
                    }
                }
                else
                {
                    print(outfile,format(lineNum,locctr,line));

                    string label=line[0];
                    if(!label.empty())
                    {
                        symOb*symbol;
                        symbol=symTab.find(label)->second;
                        if(symbol==NULL)
                        {
                            symTab[label]=new symOb(locctr,true);
                        }
                        else
                        {
                            string error="*** duplicate symbol";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                    }
                    ID *opc;
                    bool formatFour = false;
                    if(opcode[0]=='+')
                    {
                        opcode=opcode.substr(1,opcode.size()-1);
                        formatFour = true;
                    }
                    opc=opTab.find(opcode)->second;
                    if(opc!=NULL)
                    {
                        if(operand.empty())
                        {
                            if(opcode.compare("RSUB")!=0 && opc->getNumBytes()!=1)
                            {
                                string error="*** missing operand !!";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }
                        }
                        else if(operand[0]=='=')
                        {
                            string sub = operand.substr(1,operand.length()-1);
                            int checkk=checkByte(sub);
                            if(checkk==-1)
                            {
                                string error="*** invalid operand !!";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }
                            else
                            {
                                litTab.push(operand);
                            }
                        }
                        if(formatFour &&opc->getNumBytes()==3)
                        {
                            locctr = locctr + 4;
                        }
                        else
                        {
                            if(formatFour &&opc->getNumBytes()!=3)
                            {
                                string error="*** Invalid format !!";
                                print(outfile,errorFormat(error));
                                ok=false;
                            }
                            locctr=locctr+(opc->getNumBytes());
                        }
                    }
                    else if(opcode.compare("WORD")==0)
                    {
                        if(formatFour )
                        {
                            string error="*** Invalid format !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        if(checkWord(operand))  //check word
                        {
                            locctr=locctr+3;
                        }
                        else
                        {
                            string error="*** invalid operand in word directive !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                    }
                    else if(opcode.compare("BYTE")==0)
                    {
                        if(formatFour )
                        {
                            string error="*** Invalid format !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        int number=checkByte(operand);
                        if(number==-1)
                        {
                            string error="*** invalid operand in byte directive !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            locctr=locctr+number;
                        }
                    }
                    else if(opcode.compare("RESW")==0)
                    {
                        if(formatFour )
                        {
                            string error="*** Invalid format !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        int number=checkReserve(operand);
                        if(number==-1)  //check word
                        {
                            string error="*** invalid operand in RESW directive !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            locctr=locctr+(number*3);
                        }
                    }
                    else if(opcode.compare("RESB")==0)
                    {
                        if(formatFour )
                        {
                            string error="*** Invalid format !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        int number=checkReserve(operand);
                        if(number==-1)  //check word
                        {
                            string error="*** invalid operand in RESB directive !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            locctr=locctr+number;
                        }
                    }
                    else if(opcode.compare("BASE") == 0)
                    {
                        if(operand.empty())
                        {
                            string error="*** invalid base !!";
                            print(outfile,errorFormat(error));
                            ok=false;
                        }
                        else
                        {
                            base = operand;
                        }
                    }
                    else if(opcode.compare("LTORG") == 0)
                    {
                        while(!litTab.empty())
                        {
                            string str=litTab.front();
                            litTab.pop();
                            string sub = str.substr(1,str.length()-1);
                            int temp=checkByte(sub);
                            //LitOb*lit;
                            // lit=litPool.find(str)->second;
                            if(litPool.find(str)==litPool.end())
                            {
                                LitOb*adder;
                                adder=new LitOb(locctr);
                                litPool[str]=adder;
                                string s[4]="";
                                s[1] = str;
                                print(outfile,uppercaseString(formatModify(locctr,s)));
                                locctr=locctr+temp;
                            }
                        }
                    }
                    else
                    {
                        string error="*** invalid operator !!";
                        print(outfile,errorFormat(error));
                        ok=false;
                    }
                }
            }
            line[0]="";
            line[1]="";
            line[2]="";
            line[3]="";
            splitSize=primaryParsing(fgetLine(infile),line);

            //line[0] = uppercaseString(line[0]);
            //line[1] = uppercaseString(line[1]);
            // line[3] = uppercaseString(line[3]);
            if ( !(isLiteral(line[0]) || isByte(line[0])) )
            {
                // cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[0] << endl;
                line[0] = uppercaseString(line[0]);
            }

            if ( !(isLiteral(line[1]) || isByte(line[1])) )
            {
                // cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[1] << endl;
                line[1] = uppercaseString(line[1]);
            }

            if ( !(isLiteral(line[2]) || isByte(line[2])) )
            {
                //cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[2] << endl;
                line[2] = uppercaseString(line[2]);
            }

            if ( !(isLiteral(line[3]) || isByte(line[3])) )
            {
                //cout << "LITERAL OR BYTE !!!!!!!!!!!!!!!!!!!!!!!!!! " << line[3] << endl;
                line[3] = uppercaseString(line[3]);
            }
            secondaryParsing(line,splitSize+1);
            opcode = line[1];
            operand = line[2];
            // cout<<"opcode  "<<opcode<<endl;
            // cout<<"operand  "<<operand<<endl;
        }
        if(opcode.compare("END")==0)
        {
            print(outfile,format(lineNum,locctr,line));
            while(!litTab.empty())
            {
                string str=litTab.front();
                litTab.pop();
                //cout<<str<<endl;
                // string sub =  split(operand,"=").at(0);
                string sub =str.substr(1,str.length()-1);
                //cout<<sub<<endl;
                int temp=checkByte(sub);
                //LitOb*lit;
                // lit=litPool.find(str)->second;
                if(litPool.find(str)==litPool.end())
                {
                    LitOb*adder;
                    adder=new LitOb(locctr);
                    litPool[str]=adder;
                    string s[4]="";
                    s[1] = str;
                    print(outfile,uppercaseString(formatModify(locctr,s)));
                    //print(outfile,uppercaseString(formatModify(locctr,str)));
                    locctr=locctr+temp;
                }
            }
        }
        else
        {
            string error="*** Invalid END statement !!";
            print(outfile,errorFormat(error));
            ok=false;
        }
        programlength=locctr-start;
    }

    string textLine()
    {
        string write="T^";
        write=write+lineStartLoc+"^";
        int length=lineObject.length()/2;
        string hexa=hexaChange(length);
        string sizerr="";
        if(hexa.length()==1)
        {
            sizerr=sizerr+"0";
        }
        sizerr=sizerr+hexa;
        write=write+sizerr+"^";
        write=write+lineObject;
        lineObject="";
        return write;
    }
    string header()
    {
        string write="H^";
        if(startLabel.length()<6)
        {
            for(int i=0; i<6-startLabel.length(); i++)
            {
                startLabel=startLabel+" ";
            }
        }
        write=write+startLabel+"^";
        write=write+starttloc+"^";
        string leng=hexaChange(programlength);
        string tempoo="";
        for(int i=0; i<6-leng.length(); i++)
        {
            tempoo=tempoo+"0";
        }
        tempoo=tempoo+leng;
        write=write+tempoo;
        return write;
    }
    string ends()
    {
        string write="E^";
        write=write+starttloc;
        return write;
    }

    string pccounter(string operand,string next,string a [])
    {
        string error="";
        string chooseBase="01";
        int r=atoi(operand.c_str());
        int nextloc=hexToInt(next);
        int displacement=r-nextloc;
        if(!(displacement>=-2048&&displacement<=2047))
        {
            if(!base.empty())
            {
                symOb*symbol2;
                symbol2=symTab.find(base)->second;
                if(symbol2==NULL)
                {
                    displacement=0;
                    okTwo=false;
                    error=error+"*** Out of boundry base error!!\n";
                    //    print(outfile,errorFormat(error));

                }
                else
                {
                    int addressBase =symbol2->getAddress();
                    displacement=r-addressBase;
                    if(!(displacement>=0&&displacement<=4095))
                    {
                        displacement=0;
                         okTwo=false;
                        error=error+"*** Out of boundry!!\n";
                        // print(outfile,errorFormat(error));
                    }
                    else
                    {
                        chooseBase="10";
                    }
                }
            }
            else
            {
                displacement=0;
                 okTwo=false;
                error=error+"*** Out of boundry base error!!\n";
                // print(outfile,errorFormat(error));
            }
        }
        string rr=toString(displacement);
        a[0]=rr;
        a[1]=chooseBase;
        //cout<<"oneeeeeeeeeeeeeeeeeeeeeeee   "<<a[0]<<endl;
        // cout<<"twooooooooooooooooooooo   "<<a[1]<<endl;
        return error;
    }

    string printLineOne(string line)
    {
        string out = "";
        for ( int i = 0 ; i < 38 ; i++ )
        {
            out+=' ';
        }
        if ( line.size() != 6 )
        {
            return "";
        }
        string temp1;
        if ( line.at(1) == '1' && line.at(0) == '0' )
        {
            temp1 = "ABS";
        }
        else
        {
            if ( absolute )
            {
                temp1 = "ABS";
            }
            else
            {
                temp1 = "RELC";
            }
        }
        string par= "NIXBPE";
        string temp2 =" ";
        for ( int i=0 ; i < line.size() ; i++)
        {
            temp2 += par.at(i);
            temp2 += " = ";
            temp2 += line.at(i);
            temp2 += " ";
        }
        out+= temp1+temp2;
        return out;
    }
    string  printLineTwo(string line,string objectCode)
    {
        if ( line.size() > 69 )
            line = line.substr(0,68);
        if ( objectCode.size() == 0 )
            return line;
        for ( int i = 0 ; i < (100-line.size()) ; i++ )
        {
            line+='.';
        }
        line += " "+objectCode;
        return line;
    }


    void pass2Processing ()
    {
        string line[5]="";
        getParsedLine(line);
        string operand2 =line[2];
        modify(line);
        //cout << line[3] << "Line 3" <<endl;
        string nextLoc =line[0];
        string opcode = line[1];
        string operand = line[2];
        string objectBits=line[4];
        bits=objectBits;
        int displacement=0;
        string disp="";
        string errorr="";
        string finl="";
        while(opcode.compare("END") != 0 && eFile!=-1 )
        {
            finl="";
            displacement=0;
            disp="";
            errorr="";
            if(opcode.compare("START") == 0)
            {
                print(objFile,header());
                print(writeFile,printLineTwo(line[3],""));
            }
            else
            {
                ID *opc;
                opc=opTab.find(opcode)->second;
                if(opc!=NULL)
                {
                    int opcodeAddress=opc->getOC();
                    if(!operand.empty())
                    {
                        if(objectBits[5]=='1')
                        {
                            if(operand.compare("-1")==0)
                            {
                                LitOb*tem;
                                // cout<< tem <<"jjjjjjjjjjjjjjjjjjj  "<<operand2<<endl;
                                // litPool["ahmed"]=tem;
                                // tem=litPool.find(operand2)->second;
                                //cout<< tem <<"jjjjjjjjjjjjjjjjjjj  "<<operand2<<endl;
                                if(litPool.find(operand2)==litPool.end())
                                {
                                    errorr=errorr+"*** undefined symbol in operand!!\n";
                                    okTwo=false;
                                    //print(writeFile,errorFormat(error));
                                }
                                else
                                {
                                    tem=litPool.find(operand2)->second;
                                    int z=tem->getAddress();
                                    string dissp=hexaChange(z);
                                    string h="11";
                                    int current=hexToInt(currentLoc);
                                    current=current+1;
                                    string mody=hexaChange(current);
                                    string newer="";
                                    for(int i=0; i<6-mody.length(); i++)
                                    {
                                        newer=newer+"0";
                                    }
                                    newer="M^";
                                    newer=newer+mody;
                                    newer=newer+"^05";
                                    modified.push_back(newer);
                                    finl=formatObjectCode(opcodeAddress,h,dissp,4,-1,-1);

                                }

                            }
                            else
                            {
                                displacement=atoi(operand.c_str());

                                string disp4=hexaChange(displacement);
                                string h="11";
                                if(!(objectBits[1]==1&&objectBits[0]==0)&&!absolute)
                                {
                                    int current=hexToInt(currentLoc);
                                    current=current+1;
                                    string mody=hexaChange(current);
                                    string newer="";
                                    for(int i=0; i<6-mody.length(); i++)
                                    {
                                        newer=newer+"0";
                                    }
                                    newer="M^";
                                    newer=newer+mody;
                                    newer=newer+"^05";
                                    modified.push_back(newer);
                                }
                                finl=formatObjectCode(opcodeAddress,h,disp4,4,-1,-1);
                            }
                        }
                        else
                        {
                            if(opc->getNumBytes()==3 &&absolute&&operand.compare("-1")!=0&&objectBits[1]=='1')
                            {
                                //displacement=atoi(operand.c_str());
                                //string disp4=hexaChange(displacement);
                                //int current=hexToInt(currentLoc);
                                //current=current+1;
                                //string mody=hexaChange(current);
                                //string newer="";
                                // for(int i=0; i<6-mody.length(); i++)
                                //{
                                //   newer=newer+"0";
                                // }
                                // newer=newer+mody;
                                // newer=newer+"^05";
                                // modified.push_back(newer);

                                int tw=atoi(operand.c_str());
                                string hexaa=hexaChange(tw);
                                if(hexaa.length()>3)
                                {
                                    errorr=errorr+"*** invalid operand in immediate addressing!!\n";
                                    okTwo=false;
                                }
                                else
                                {
                                    finl=formatObjectCode(opcodeAddress,"00",hexaa,3,-1,-1);
                                }

                            }
                            else
                            {
                                string chooseBase="01";
                                if(operand.compare("-1")==0)
                                {
                                    if(opc->getNumBytes()==2)
                                    {
                                        int reegister[3];
                                        checkRegisterOperand(opcode,operand2,reegister);
                                        if(reegister[0]==0)
                                        {
                                            errorr=errorr+"*** invalid operand!!\n";
                                            okTwo=false;
                                            //   print(outfile,errorFormat(error));
                                        }
                                        else
                                        {
                                            string u="empty";
                                            finl=formatObjectCode(opcodeAddress,chooseBase,u,2,reegister[1],reegister[2]);
                                        }

                                    }
                                    else
                                    {
                                        LitOb*tem;
                                        if(litPool.find(operand2)==litPool.end())
                                        {
                                            errorr=errorr+"*** undefined symbol in operand!!\n";
                                            okTwo=false;
                                            //   print(outfile,errorFormat(error));
                                        }
                                        else
                                        {
                                            tem=litPool.find(operand2)->second;
                                            int z=tem->getAddress();
                                            string arraay[2]="";
                                            string re=toString(z);
                                            errorr=errorr+pccounter(re,nextLoc,arraay);
                                            string one=arraay[0];
                                            displacement=atoi(one.c_str());
                                            string dissp=hexaChange(displacement);
                                            chooseBase=arraay[1];
                                            finl=formatObjectCode(opcodeAddress,chooseBase,dissp,3,-1,-1);
                                        }
                                    }
                                }
                                else
                                {
                                    string arraay[2]="";
                                    errorr=errorr+pccounter(operand,nextLoc,arraay);
                                    string one=arraay[0];
                                    //cout<<"sheriiiiiif   "<<one<<endl;
                                    displacement=atoi(one.c_str());
                                    chooseBase=arraay[1];
                                    int format=0;
                                    format=opc->getNumBytes();
                                    string disp4=hexaChange(displacement);
                                    //cout<<"didplacmentttttttttttttttttt  "<<disp4<<endl;
                                    finl=formatObjectCode(opcodeAddress,chooseBase,disp4,format,-1,-1);
                                    // }
                                }
                            }
                        }

                    }
                    else
                    {
                        string relative="00";
                        //cout<<"rubbb "<<opcode<<endl;
                        if(operand2[0]=='+')
                        {
                            bits="110001";
                        }
                        else
                        {
                            bits="110000";
                        }
                        finl=formatObjectCode(opcodeAddress,relative,"000",opc->getNumBytes(),-1,-1);
                    }
                }
                else if(opcode.compare("WORD")==0)
                {
                    finl=wordObj(operand2);
                }
                else if(opcode.compare("BYTE")==0)
                {
                    finl=byteObj(operand2);
                }
                else if(opcode.compare("EQU")==0||opcode.compare("ORG")==0||opcode.compare("LTORG")==0)
                {
                    if(operand.compare("-1")==0)
                    {
                        errorr=errorr+"*** invalid operand !!\n";
                        okTwo=false;
                    }
                    //cout<<"5allllllllllllllleeeeeeeeeeeeeddddd"<<endl;
                    finl="";
                }
                // int one=hexToInt(nextLoc);
                // int two=hexToInt(currentLoc);
                //  int result=one-two;
                //  string resultt=hexaChange(result);
                int totall=0;
                if(opcode.compare("RESW")==0||opcode.compare("RESB")==0)
                {
                    if(!lineObject.empty())
                    {
                        print(objFile,textLine());
                        lineObject="";
                        lineStartLoc=nextLoc;
                    }
                    else
                    {
                        lineStartLoc=nextLoc;
                    }
                }
                else
                {
                    totall=lineObject.length()+finl.length();
                    if(totall>60)
                    {
                        print(objFile,textLine());
                        lineObject="";
                        lineStartLoc=currentLoc;
                        lineObject=lineObject+finl;
                    }
                    else
                    {
                        lineObject=lineObject+finl;
                    }
                }
                /*
                if(resultt.length()>2)
                {
                    totall=lineObject.length()+finl.length();
                    if(totall>60)
                    {
                        print(objFile,textLine());
                        lineObject="";
                        lineStartLoc=currentLoc;
                        lineObject=lineObject+finl;
                    }
                    else
                    {
                        lineObject=lineObject+finl;
                        print(objFile,textLine());
                        lineObject="";
                        lineStartLoc=nextLoc;

                    }
                }
                else
                {
                    totall=lineObject.length()+finl.length();
                    if(totall>60)
                    {
                        print(objFile,textLine());
                        lineObject="";
                        lineStartLoc=currentLoc;
                        lineObject=lineObject+finl;
                    }
                    else
                    {
                        lineObject=lineObject+finl;
                        // print(objFile,textLine());
                        // lineObject="";
                        // lineStartLoc=nextLoc;

                    }
                }
                */
                print(writeFile,printLineOne(bits));
                if(errorr.empty())
                {
                    print(writeFile,printLineTwo(line[3],finl));
                }
                else
                {
                    print(writeFile,printLineTwo(line[3],""));
                    print(writeFile, errorFormat(errorr.substr(0,errorr.length()-1)));
                }

            }
            line[0]="";
            line[1]="";
            line[2]="";
            line[3]="";
            line[4]="";
            getParsedLine(line);
            operand2 =line[2];
            string th=line[1];
            //cout<<"hhhhhhhhhhhhhhhhhhhhhhhhhhhhh  "<<th<<endl;
            if(th.compare("WORD")!=0&&th.compare("BYTE")!=0&&th.compare("RESB")!=0&&th.compare("RESW")!=0&&th.compare("END")!=0&&th.compare("RSUB")!=0&&th.compare("LTORG")!=0)
            {
                //cout << "Do not modify this opcode " << th << endl;
                modify(line);

            }
            else
            {
                absolute = false;
            }
            nextLoc =line[0];
            opcode = line[1];
            operand = line[2];
            objectBits=line[4];
            bits=objectBits;
        }
        if(opcode.compare("END")==0)
        {
            if(lineObject.length()>0)
            {
                print(objFile,textLine());
                //cout<<"laaaast  "<<lineObject<<endl;
                lineObject="";
            }
            for(int i=0; i<modified.size(); i++)
            {
                print(objFile,modified.at(i));
            }
            print(objFile,ends());
            print(writeFile,printLineTwo(line[3],""));
        }
        cout << "   Pass 2 Complete !!";
    }

    bool inHashMap(string opcode)
    {
        ID * R;

        R = opTab.find(opcode)->second;

        if(R == NULL)
        {
            return false;
        }
        else
        {
            return true;
        }

    }

    bool inSymTab(string symbol)
    {
        symOb * R;

        R = symTab.find(symbol)->second;

        if(R == NULL)
        {
            return false;
        }
        else
        {
            return true;
        }

    }

    int getAddress(string label)
    {
        return symTab.find(label)->second->getAddress();
    }



    int getFormat(string opcode)
    {
        ID * R;

        R = opTab.find(opcode)->second;

        if(R == NULL)
        {
            return -1;
        }
        else
        {
            return R->getNumBytes();
        }

    }


    void Map()
    {
        ID *element;
        element = new ID(24,3);
        opTab["ADD" ] = element;
        element = new ID(88,3);
        opTab["ADDF" ] = element;
        element = new ID(144,2);
        opTab["ADDR" ] = element;
        element = new ID(64,3);
        opTab["AND" ] = element;
        element = new ID(180,2);
        opTab["CLEAR" ] = element;
        element = new ID(40,3);
        opTab["COMP" ] = element;
        element = new ID(136,3);
        opTab["COMPF" ] = element;
        element = new ID(160,2);
        opTab["COMPR" ] = element;
        element = new ID(36,3);
        opTab["DIV" ] = element;
        element = new ID(100,3);
        opTab["DIVF" ] = element;
        element = new ID(156,2);
        opTab["DIVR" ] = element;
        element = new ID(196,1);
        opTab["FIX" ] = element;
        element = new ID(192,1);
        opTab["FLOAT" ] = element;
        element = new ID(244,1);
        opTab["HIO" ] = element;
        element = new ID(60,3);
        opTab["J" ] = element;
        element = new ID(48,3);
        opTab["JEQ" ] = element;
        element = new ID(52,3);
        opTab["JGT" ] = element;
        element = new ID(56,3);
        opTab["JLT" ] = element;
        element = new ID(72,3);
        opTab["JSUB" ] = element;
        element = new ID(0,3);
        opTab["LDA" ] = element;
        element = new ID(104,3);
        opTab["LDB" ] = element;
        element = new ID(80,3);
        opTab["LDCH" ] = element;
        element = new ID(112,3);
        opTab["LDF" ] = element;
        element = new ID(8,3);
        opTab["LDL" ] = element;
        element = new ID(108,3);
        opTab["LDS" ] = element;
        element = new ID(116,3);
        opTab["LDT" ] = element;
        element = new ID(4,3);
        opTab["LDX" ] = element;
        element = new ID(208,3);
        opTab["LPS" ] = element;
        element = new ID(32,3);
        opTab["MUL" ] = element;
        element = new ID(96,3);
        opTab["MULF" ] = element;
        element = new ID(152,2);
        opTab["MULR" ] = element;
        element = new ID(200,1);
        opTab["NORM" ] = element;
        element = new ID(68,3);
        opTab["OR" ] = element;
        element = new ID(216,3);
        opTab["RD" ] = element;
        element = new ID(172,2);
        opTab["RMO" ] = element;
        element = new ID(76,3);
        opTab["RSUB" ] = element;
        element = new ID(164,2);
        opTab["SHIFTL" ] = element;
        element = new ID(168,2);
        opTab["SHIFTR" ] = element;
        element = new ID(240,1);
        opTab["SIO" ] = element;
        element = new ID(236,3);
        opTab["SSK" ] = element;
        element = new ID(12,3);
        opTab["STA" ] = element;
        element = new ID(120,3);
        opTab["STB" ] = element;
        element = new ID(84,3);
        opTab["STCH" ] = element;
        element = new ID(128,3);
        opTab["STF" ] = element;
        element = new ID(212,3);
        opTab["STI" ] = element;
        element = new ID(20,3);
        opTab["STL" ] = element;
        element = new ID(124,3);
        opTab["STS" ] = element;
        element = new ID(232,3);
        opTab["STSW" ] = element;
        element = new ID(132,3);
        opTab["STT" ] = element;
        element = new ID(16,3);
        opTab["STX" ] = element;
        element = new ID(28,3);
        opTab["SUB" ] = element;
        element = new ID(92,3);
        opTab["SUBF" ] = element;
        element = new ID(148,2);
        opTab["SUBR" ] = element;
        element = new ID(176,2);
        opTab["SVC" ] = element;
        element = new ID(224,3);
        opTab["TD" ] = element;
        element = new ID(248,1);
        opTab["TIO" ] = element;
        element = new ID(44,3);
        opTab["TIX" ] = element;
        element = new ID(184,2);
        opTab["TIXR" ] = element;
        element = new ID(220,3);
        opTab["WD" ] = element;
        element = new ID(-1,1);
    }

    void getParsedLine(string out[])
    {
        out[3] = current;
        if ( out[3].at(0) ==' ')
        {
            current = fgetLine(readFile);
            return;
        }
        if(out[3].at(0) =='>')
        {
            eFile = -1;
            return ;
        }
        int length=0;
        for ( int i = 26 ; i <current.size() ; i++)
        {
            if ( current.at(i) ==' ' )
            {
                break;
            }
            length++;
        }
        out[1] = current.substr(26,length);
        length=0;
        for (int i = 38 ; i < current.size(); i++)
        {
            if ( current.at(i) ==' ' )
            {
                break;
            }
            length++;
        }
        out[2] = current.substr(38,length);
        currentLoc = current.substr(6,6);
        current = fgetLine(readFile);
        while ( current.at(0)==' ' )
        {
            print(writeFile,current);
            current = fgetLine(readFile);
        }
        out[0] = current.substr(6,6);


        //  cout <<"\n\n" << out[2] << "\n\n";

    }

    vector<string> split(string str1,string sf)
    {
        vector<string> f;
        char * pch;
        for ( int i = 0 ; i < str1.length() ; i++ )
        {
            if ( i == 23 )
            {
                //  cout << "";
            }

            if( str1.at(i) == '*' )
            {
                if( i < str1.size()-1 && str1.at(i+1) > 41 && str1.at(i+1) < 48)
                {
                    if ( i!=0 && ((str1.at(i-1) > 41 && str1.at(i-1) < 48)||str1.at(i-1) ==' ' ) )
                    {
                        str1 = str1.substr(0,i)+"&&&"+str1.substr(i+1,str1.length());
                        i+=2;
                    }

                }
                if ( i == 0 || i == str1.size()-1 )
                {
                    str1 = str1.substr(0,i)+"&&&"+str1.substr(i+1,str1.length());
                    i+=2;
                }
            }
            if ( str1.at(i) > 41 && str1.at(i) < 48 || str1.at(i)=='(' || str1.at(i)==')' )
            {
                str1 = str1.substr(0,i)+' '+str1.at(i)+' '+str1.substr(i+1,str1.length());
                i++;
            }
        }
        char str[100];
        strcpy(str,str1.c_str());
        pch = strtok (str,sf.c_str());
        while (pch != NULL)
        {
            f.push_back(pch);
            pch = strtok (NULL, sf.c_str());
        }
        return f;
    }
    string infixToPostfix(vector<string> str)
    {
        for ( int i = 0 ; i < str.size() ; i++ )
        {
            // cout << str.at(i) << "  ";
        }
        // cout << endl;
        stack<string> stack;
        string postFix="";
        int comingPr=0;
        int topPr=0;
        for ( int i = 0 ; i < str.size() ; i++ )
        {
            if( str.at(i).at(0)=='(' )
            {
                stack.push(str.at(i));
            }
            else if(str.at(i).at(0) ==')')
            {
                while( stack.top().at(0) !='(' )
                {
                    postFix += stack.top()+" ";
                    stack.pop();
                }
                stack.pop();
            }
            else if ( str.at(i).at(0) > 41 && str.at(i).at(0) < 48  )
            {
                if ( str.at(i).at(0) == '+' || str.at(i).at(0) == '-' )
                {
                    comingPr=1;
                }
                else
                {
                    comingPr=2;
                }
                if ( !stack.empty() && (stack.top().at(0) == '+' || stack.top().at(0) == '-') )
                {
                    topPr =1;
                }
                else if(!stack.empty() && (stack.top().at(0) == '*' || stack.top().at(0) == '/') )
                {
                    topPr =2;
                }
                if ( topPr < comingPr || stack.top().at(0) == '(' )
                {
                    stack.push(str.at(i));
                }
                else
                {
                    while( !stack.empty() && topPr >= comingPr )
                    {
                        postFix += stack.top()+" ";
                        stack.pop();
                        if ( !stack.empty() && ( stack.top().at(0) == '+' || stack.top().at(0) == '-' ) )
                        {
                            topPr =1;
                        }
                        else if ( !stack.empty() && (stack.top().at(0) == '*' || stack.top().at(0) == '/') )
                        {
                            topPr =2;
                        }
                    }
                    stack.push(str.at(i));
                }
            }
            else
            {
                postFix += str.at(i)+" ";
            }
            // cout << postFix << endl;
        }
        while( !stack.empty() )
        {
            postFix += stack.top()+" ";
            stack.pop();
        }
        return postFix;
    }


    int primaryParsing(string inputLine, string * line)
    {


        bool spaceFound = false;
        bool commentField = false;
        int arrayPointer = 0;
        int actualArrayPointer = -1;
        bool actualWrite = false;
        int i=0;
        if(inputLine.empty())
        {
            return -1;
        }
        if(inputLine.at(0) == ' ')
        {
            spaceFound = true;
            actualWrite = false;
            i++;
        }

        for(; i<inputLine.size() ; i++)
        {

            if(inputLine.at(i) == ' ' && !spaceFound && !commentField)
            {
                spaceFound = true;
                arrayPointer++;
                actualWrite = false;

            }
            else if(inputLine.at(i) == ' ' && spaceFound && !commentField)
            {
                actualWrite = false;
                continue;
            }
            else if(inputLine.at(i) == '\t')
            {
                continue;
            }
            else
            {
                //    concatinate on current array pointer;

                if(inputLine.at(i) == '.')
                {
                    //        put rest of the string in line[3]
                    line[3] = inputLine.substr (i,inputLine.size()-i);
                    return 0;
                }


                if(arrayPointer == 3)
                {
                    commentField = true;
                }
                if(!actualWrite)
                {
                    actualWrite = true;
                    actualArrayPointer++;
                }
                line[arrayPointer] += inputLine.at(i);
                spaceFound = false;
            }

        }
        for(int i = 0 ; i < 4 ; i++)
        {
            //cout << line[i] << " " << i << endl;
        }
        //cout << actualArrayPointer << endl;
        return actualArrayPointer;
    }
    void secondaryParsing (string Field [] , int splitSize)
    {
        string temp = "";
        bool foundInOpTab = false;
        // split = 4 but actually is 3 or 2 : field [0] is in optab? problem: noproblem
        // problem : format 1 or rsub => 1 opcode & 3 comments , else : opcode , operand and 2 comment
        // if field [1] is opcode then  label , opcode , comment (in case format 1 or rsub)
        // split = 3 but actually 2 : opcode and 2 comments if format 1 or rsub
        if (splitSize == 4)
        {
            string operate = Field[0];
            if (Field[0][0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }
            if (inHashMap(operate) || Field[0] == "BYTE" || Field[0] == "WORD" || Field[0] == "RESW" || Field[0] == "RESB" || Field[0] == "ORG" || Field[0] == "BASE" || Field[0] == "LTORG" || Field[0] == "EQU" || Field[0] == "END" || Field[0] == "START")
            {
                string temp = "";
                string operate2 = Field[1];
                if (operate2[0] == '+')
                {
                    operate2 =operate2.substr(1,operate2.size()-1);
                }

                if (operate == "END" && (inHashMap(operate2) || Field[1] == "BYTE" || Field[1] == "WORD" || Field[1] == "RESW" || Field[1] == "RESB" || Field[1] == "ORG" || Field[1] == "BASE" || Field[1] == "LTORG" || Field[1] == "EQU"))
                {

                    if (operate2 == "RSUB" || getFormat(operate2) == 1 || operate2 == "LTORG" )
                    {
                        temp = Field[2]+" "+Field[3];
                        Field [2] = "";
                        Field [3] = temp;
                    }
                }
                else if (operate == "END" && Field[1] == "END")
                {
                    if (!inSymTab(Field[2]))
                    {

                        temp = Field[2]+" "+Field[3];
                        Field [2] = "";
                        Field [3] = temp;

                    }
                }

                else if (Field[0] == "RSUB" || getFormat(Field[0] ) == 1 || Field[0] == "LTORG")
                {

                    temp = Field[1]+" "+Field[2]+" "+Field[3];
                    Field [1] = Field [0];
                    Field [0] = "";
                    Field [2] = "";
                    Field [3] = temp;
                }
                else
                {
                    temp = Field[2]+" "+Field[3];
                    Field [3] = temp;
                    Field [2] = Field [1];
                    Field [1] = Field [0];
                    Field [0] = "";
                }
            }

            operate = Field[1];
            if (operate[0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }
            if (inHashMap(operate) || Field[1] == "BYTE" || Field[1] == "WORD" || Field[1] == "RESW" || Field[1] == "RESB" || Field[1] == "ORG" || Field[1] == "BASE" || Field[1] == "LTORG" || Field[1] == "EQU" || Field[1] == "END" || Field[1] == "START")
            {
                string temp = "";
                if (Field[1] == "RSUB" || getFormat(Field[1] ) == 1 || Field[1] == "LTORG")
                {
                    temp = Field[2]+" "+Field[3];
                    Field [2] = "";
                    Field [3] = temp;
                }
                else if (Field[1] == "END")
                {
                    if (!inSymTab(Field[2]))
                    {
                        temp = Field[2]+" "+Field[3];
                        Field [2] = "";
                        Field [3] = temp;
                    }
                }
            }
        }

        else if (splitSize == 3)
        {

            string operate = Field[0];
            if (operate[0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }

            if (inHashMap(operate)|| Field[0] == "BYTE" || Field[0] == "WORD" || Field[0] == "RESW" || Field[0] == "RESB" || Field[0] == "ORG" || Field[0] == "BASE" || Field[0] == "LTORG" || Field[0] == "EQU" || Field[0] == "END" || Field[0] == "START")
            {
                string temp = "";
                string operate2 = Field[1];
                if (operate2[0] == '+')
                {
                    operate2 =operate2.substr(1,operate2.size()-1);
                }

                if (Field [0] == "END" && (inHashMap(operate2) || Field[1] == "BYTE" || Field[1] == "WORD" || Field[1] == "RESW" || Field[1] == "RESB" || Field[1] == "ORG" || Field[1] == "BASE" || Field[1] == "LTORG" || Field[1] == "EQU"))
                {
                    if (operate2 == "RSUB" || getFormat(operate2) == 1 || operate2 == "LTORG" )
                    {
                        Field[3] =Field[2];
                        Field [2] = "";
                    }
                }
                else if (Field [0] == "END" && Field[1] == "END" )
                {

                    if (!(inSymTab(Field[2])))
                    {
                        Field[3] =Field[2];
                        Field [2] = "";
                    }
                }

                else if (Field[0] == "RSUB" || getFormat(Field[0] ) == 1 || Field[0] == "LTORG") // SOMETHING TAKES NO OPERAND
                {
                    temp = Field[1]+" "+Field[2];
                    Field [1] = Field [0];
                    Field [0] = "";
                    Field [2] = "";
                    Field [3] = temp;
                }
                else  // opcode, operand, comment
                {
                    Field [3] = Field [2];
                    Field [2] = Field [1];
                    Field [1] = Field [0];
                    Field [0] = "";
                }
            }

            operate = Field[1];
            if (Field[1][0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }
            if(inHashMap(operate) || Field[1] == "BYTE" || Field[1] == "WORD" || Field[1] == "RESW" || Field[1] == "RESB" || Field[1] == "ORG" || Field[1] == "BASE" || Field[1] == "LTORG" || Field[1] == "EQU" || Field[1] == "START")
            {
                if (Field[1] == "RSUB" || getFormat(operate ) == 1 || Field[1] == "LTORG") // SOMETHING TAKES NO OPERAND
                {
                    Field [3] = Field [2];
                    Field [2] = "";
                }
            }
            else if (Field[1] == "END")
            {

                if (!inSymTab(Field[2]))
                {
                    Field [3] = Field[2] ;
                    Field[2] = "";
                }
            }
            // else : label , opcode ,operand so no problem
        }

        else if (splitSize == 2)
        {

            string temp = "";
            string operate = Field[0];
            // cout<<"Split size 2 : "<< operate<<endl;
            if (Field[0][0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }
            if (inHashMap(operate) || Field[0] == "BYTE" || Field[0] == "WORD" || Field[0] == "RESW" || Field[0] == "RESB" || Field[0] == "ORG" || Field[0] == "BASE" || Field[0] == "LTORG" || Field[0] == "EQU" || Field[0] == "END" || Field[0] == "START")
            {
                string operate2 = Field[1];
                if (operate2[0] == '+')
                {
                    operate2 =operate2.substr(1,operate2.size()-1);
                }

                if (operate == "END" && (Field[1] == "END" )) // END END
                {
                    // do nothing it i set correctly
                    //END END
                }
                else if (operate == "END" && inSymTab(operate2) )  // END TEST
                {
                    Field [2] = Field [1];
                    Field [1] = Field [0];
                    Field [0] = "";
                    Field [3] = "";

                }
                else if (operate == "END" && inHashMap(operate2))
                {
                    // End RSUB
                }

                else if (Field[0] == "RSUB" || getFormat(operate) == 1 || Field[0] == "LTORG")
                {
                    // opcode , comment
                    temp = Field[1];
                    Field [1] = Field [0];
                    Field [0] = "";
                    Field [2] = "";
                    Field [3] = temp;
                }
                else if (operate == "END" && !(inSymTab(Field[1])))
                {

                    Field[3] = Field[1];
                    Field[1] = "";

                }
                else  // opcode, operand
                {
                    Field [2] = Field [1];
                    Field [1] = Field [0];
                    Field [0] = "";
                    Field [3] = "";
                    // cout << Field [1] <<endl;
                }
            }
        }


        else if (splitSize == 1)
        {
            //  RSUB and format 1


            string operate = Field[0];
            // cout<<"Split size 1 : "<< operate<<endl;
            if (operate[0] == '+')
            {
                operate=operate.substr(1,operate.size()-1);
            }
            if (inHashMap(operate) || operate == "END" || operate == "LTORG" || operate == "BASE")
            {
                Field [1] = Field [0];
                Field [0] = "" ;
                Field [2] = "";
                Field [3] = "";
            }
            else
            {
                Field [0] = "";
                Field [1] = "";
                Field [2] = "";

            }
        }
        else if (splitSize == 0)
        {
            Field [0] = "";
            Field [1] = "";
            Field [2] = "";
            Field [3] = "";
            // empty line
        }

    }


    string uppercaseString(string s)
    {
        char c;
        int i=0;
        while (s[i])
        {
            c=s[i];
            s[i] = (char) toupper(c);
            i++;
        }
        return s;
    }

    string wordObj(string str)
    {
        string temp = hexaChange(atoi( str.c_str()));
        string zero ="";
        for ( int i =0 ; i < 6-temp.size(); i++ )
        {
            zero += '0';
        }
        return zero+temp;
    }

    string byteObj( string str )
    {
        if( str.at(0) == 'X' )
        {
            vector<string> s = split(str,"'X");
            return s[0];
        }
        else if( str.at(0) == 'C' )
        {
            str = str.substr(1,str.length());
            vector<string> s = split(str,"'");
            string out = "";
            for ( int i = 0 ; i < s[0].size() ; i++ )
            {
                int c = (int) (s[0].at(i));
                out += hexaChange(c);
            }
            return out;
        }
    }

    void print(FILE* pFile,string s)
    {
        fputs((s+"\n").c_str(),pFile);
    }

    string toString(int i)
    {
        std::stringstream stream;
        stream << i;
        std::string result( stream.str() );
        return result;
    }

    string hexaChange ( long s)
    {
        std::stringstream stream;
        stream << std::hex << s;
        std::string result( stream.str() );
        return result;
    }

    string commentFormat(string comment)
    {
        string out= "";
        for ( int i = 0 ; i < 15 ; i++)
        {
            out+=' ';
        }
        out+= comment;
        return out;
    }

    string errorFormat(string error)
    {
        string out= "";
        for ( int i = 0 ; i < 26 ; i++)
        {
            out+=' ';
        }
        out+= "Error:"+error;

        return out;
    }



    string formatModify( int loc , string s [] )
    {
        string out=""; // the output string
        string locStr = hexaChange(loc);// loc counter in string
        //out += lineStr; // add line value to first for out string
        // for loop to reserve the space in various length for string
        for ( int i = 0 ; i < 6  ; i++)
        {
            out+=' ';
        }

        for ( int i = 0 ; i < (6 - locStr.size()) ; i++)
        {
            out+='0';
        }
        out += locStr+"   ";
        out += s[0]; // Label value
        for ( int i = 0 ; i < (10- s[0].size()); i++ )
        {
            out +=' ';
        }
        out+=' ';
        out += s[1];// Mnemonic value
        for ( int i = 0 ; i < (10- s[1].size()); i++ )
        {
            out +=' ';
        }
        out+="  ";
        out += s[2];// Operand value
        for ( int i = 0 ; i < (30 - s[2].size()) ; i++)
        {
            out +=' ';
        }
        out+=s[3]; // comment value
        lineNum++;
        return out;
    }



    string format( int line , int loc , string s [] )
    {
        string out=""; // the output string
        string lineStr = toString(line);// line number in string
        string locStr = hexaChange(loc);// loc counter in string
        out += lineStr; // add line value to first for out string
        // for loop to reserve the space in various length for string
        for ( int i = 0 ; i < (6 - lineStr.size()) ; i++)
        {
            out+=' ';
        }

        for ( int i = 0 ; i < (6 - locStr.size()) ; i++)
        {
            out+='0';
        }
        out += locStr+"   ";
        out += s[0]; // Label value
        for ( int i = 0 ; i < (10- s[0].size()); i++ )
        {
            out +=' ';
        }
        out+=' ';
        out += s[1];// Mnemonic value
        for ( int i = 0 ; i < (10- s[1].size()); i++ )
        {
            out +=' ';
        }
        out+="  ";
        out += s[2];// Operand value
        for ( int i = 0 ; i < (30 - s[2].size()) ; i++)
        {
            out +=' ';
        }
        out+=s[3]; // comment value
        lineNum++;
        return out;
    }

    unsigned int hexToInt(string hex)
    {
        unsigned int x;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> x;
        return x;
    }


    void init()
    {
        print(outfile,"Output");
        string temp ="#";
        temp +="     Address";
        temp +="  Label ";
        temp +="     Mnemonic ";
        temp +="   Operand ";
        temp +="                      Comments ";
        print(outfile,temp);
    }
    string fgetLine(FILE* file)
    {
        char str[100];
        if ( fgets (str , 100 , infile) == NULL )
        {
            eFile = -1;
        }
        string d(str);
        unsigned  pos = d.find("\n");
        d =d.substr (0,pos);
        return d;
    }

    bool checkWord(string word)
    {
        int i = 0;

        if(word[i] == '-')
        {
            i++;
        }

        for( ; i < word.size() ; i++)
        {

            if(word[i] > '9' || word[i] < '0')
            {
                return false;
            }
        }
        return true;
    }

    int checkByte(string byte)
    {

        if(byte[0] != 'C' && byte[0] != 'c' && byte[0] != 'X' && byte[0] != 'x')
        {
            return -1;
        }

        if(byte[1] != '\'' )
        {
            return -1;
        }

        if(byte[byte.size() - 1] != '\'' )
        {
            return -1;
        }

        // a character input
        if(byte[0] == 'C' || byte[0] == 'c')
        {
            // check
            return byte.size()-3;
        }


        // a hex word input
        int numberOfHalfBytes = 0 ;
        if(byte[0] == 'X' || byte[0] == 'x')
        {

            for(int i = 2 ; i < byte.size()-1 ; i++)
            {
                if(((byte[i] >= '0')&&(byte[i] <= '9'))||((byte[i] >= 'a')&&(byte[i] <= 'f'))||((byte[i] >= 'A')&&(byte[i] <= 'F')))
                {

                    numberOfHalfBytes++;
                }
                else
                {
                    return -1;
                }
            }
            if(numberOfHalfBytes % 2 == 1)
            {
                return -1;
            }
            return numberOfHalfBytes/2;
        }
        return -1;
    }

    int checkReserve(string res)
    {
        int i = 0;
        if(res[i] == '-')
        {
            i++;
            return -1;
        }
        for ( ; i < res.size() ; i++)
        {
            if(res[i] < '0' || res[i] > '9' )
            {
                return -1;
            }
        }
        int temp = atoi (res.c_str());
        return temp;
    }

    string firstRow()
    {
        string out="";
        for ( int i = 0 ; i < 5 ; i++)
        {
            out+=' ';
        }
        out+="Name";
        for ( int i = 0 ; i < 8 ; i++)
        {
            out+=' ';
        }
        out+="value";
        for ( int i = 0 ; i < 6 ; i++)
        {
            out+=' ';
        }
        out+=" Absol/Reloc";
        return out;
    }

    string rowTableFormat(string name,int address , int r)
    {
        string out="";
        for ( int i = 0 ; i < 5 ; i++)
        {
            out+=' ';
        }
        string addstr = hexaChange(address);
        string reAbs = (r == 1)? "Relocatable" : "Absolute";
        out += name;
        for ( int i = 0 ; i < (12 - name.length()) ; i++)
        {
            out+=' ';
        }
        out+=addstr;
        for ( int i = 0 ; i < (12 - addstr.size()) ; i++)
        {
            out+=' ';
        }
        out += reAbs;
        return out;
    }

    void printSymTab()
    {
        map<string,symOb*>::iterator it;
        for (map<string,symOb*>::iterator it=symTab.begin(); it!=symTab.end(); ++it)
        {
            symOb* symTemp = it->second;
            string name =it->first;
            int temp1 = symTemp->getAddress();
            int temp2 = symTemp->getRelocatable();

            print(outfile,rowTableFormat(name,temp1,temp2));
        }
    }




    int initFiles(int argc, char *argv[])
    {
        if ( argc > 1)
        {
            string filename(argv[1]);
            infile = fopen((filename+".txt").c_str(),"r");
        }
        else
        {
            infile = fopen("T.txt","r");
        }
        outfile = fopen ("LISAFILE.txt","w");
        if(infile == NULL)
        {
            cout << " File Can't Open !! ";
            return 0;
        }
        //    cout <<"\n\n   successfully loaded ( File : "<< filename<<" )\n\n"<<endl;
        return 1;
    }

    int stringToInteger(string number)
    {
        int ten = 1;
        int i = number.length()-1;
        int temp;
        int result =0;

        for( ; i >= 0 ; i--)
        {
            temp = number.at(i)-48;
            result += temp*ten;
            ten*=10;
        }
        return result;
    }

    int modify(string line [])
    {
        //string field_3 line[1]operation, string field_4 operand




        short bits [6];//= new short [6]; //intialize to 0 , dont listen to tarek
        for(int i = 0 ; i < 6 ; i++)
        {
            bits[i] = 0;
        }

        // modification of field 3
        //field 3 nedeef string = no '+'
        if(line[1].at(0) == '+')
        {
            line[1] = line[1].substr(1,line[1].length()-1);
            bits[5] = 1;
        }


        // modification of field 3
        // field 4 nedeef string = no '@' // no '#' // no ',X' // no expressions

        if(line[2].at(0) == '@')
        {
            line[2] = line[2].substr(1,line[2].length()-1);
            bits[0] = 1;
        }
        else if(line[2].at(0) == '#')
        {
            line[2] = line[2].substr(1,line[2].length()-1);
            bits[1] = 1;
        }
        else
        {
            bits[0] = 1;
            bits[1] = 1;
        }

        int field_4_length = line[2].length();
        //check for ,X
        if(line[2].find_last_of(",") == field_4_length-2)
        {
            if(line[2].at(field_4_length-1) == 'X' ||  line[2].at(field_4_length-1) == 'x')
            {
                line[2] = line[2].substr(0,field_4_length-2);
                bits[2] = 1;
            }
        }
        //check for expression
        // call omar's method infix to postfix expression
        //cout << line[2]<<endl;
        string pf = infixToPostfix(split(line[2]," "));
        int address = evaluatePostFix(pf);
        line[2] = toString(address);

        string bitsString = "";
        for(int i = 0 ; i < 6 ; i++)
        {
            if(bits[i] == 0)
            {
                bitsString+="0";
            }
            else
            {
                bitsString+="1";
            }
        }
        line[4] = bitsString;

    }


    int evaluatePostFix(string postfix)
    {
        // parse on space and push in stack
        // maintain an int at which

        stack <int> value;
        stack <int> relativity;
        string temp = "";
        bool isNumber = true;
        int firstR;
        int secondR;
        int firstV;
        int secondV;
        char current;
        bool sign = false;
        int address;
        absolute = false;



        if(postfix == "*" || postfix == "* ")
        {
            int currrrent=hexToInt(currentLoc);

            return currrrent;
        }

        for(int i = 0 ; i < postfix.length() ; i++)
        {
            current = postfix.at(i);

            if(postfix.at(i) == ' ')
            {
                // do the following
                // check if it was a number or not

                if(isNumber && !sign)
                {
                    // a number
                    // if it was push it in the stack as is with relativity = 0

                    // stringstream used for the conversion initialized with the contents of Text

                    //put temp as an number
                    value.push(stringToInteger(temp));

                    relativity.push(0);

                }
                else if(!isNumber && !sign) //a label
                {
                    // if it was a string get it from the symTab then push it with relativity = 1
                    //temp's value from ths symtab


                    /*
                     symOb* symTemp = it->second;
                            string name =it->first;
                            int temp1 = symTemp->getAddress();
                            int temp2 = symTemp->getRelocatable();
                    */

                    //                bool relocatable = symTab.find(temp)->second->getRelocatable();

                    if(temp == "&&&" || temp == "&&& ")
                    {
                        //cout << hexToInt(currentLoc)<<  "&&&&&&&&&&&&&&&&&&&&&&&&&&&& + ." << temp << "."<<currentLoc << endl;
                        //value.push(hexToInt(currentLoc));
                        //cout << locctr << endl;
                        value.push(locctr);
                        relativity.push(1);

                    }
                    else
                    {
                        symOb*tr;
                        tr=symTab.find(temp)->second;
                        if(tr == NULL)
                        {
                            //cout << "getrelocatable == null" << endl;
                            return -1;
                        }
                        bool relocatable = tr->getRelocatable();

                        address = getAddress(temp);
                        if(address == NULL)
                        {
                            return -1;
                        }

                        value.push(address);

                        if(relocatable)
                            relativity.push(1);
                        else
                            relativity.push(0);

                    }
                }

                // intialize variables again intialize temp
                isNumber = true;
                temp = "";
            }
            else if(postfix.at(i) == '+')
            {

                sign = true;
                // pop the last two
                if(!relativity.empty())
                {
                    secondR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!relativity.empty())
                {
                    firstR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }

                relativity.push(secondR+firstR);
                if(!value.empty())
                {
                    secondV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    firstV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }

                value.push(secondV+firstV);

            }
            else if(postfix.at(i) == '-')
            {
                sign = true;
                if(!relativity.empty())
                {
                    secondR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!relativity.empty())
                {
                    firstR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    secondV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    firstV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }

                relativity.push(firstR-secondR);

                value.push(firstV-secondV);

            }
            else if(postfix.at(i) == '*')
            {
                sign = true;
                if(!relativity.empty())
                {
                    secondR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!relativity.empty())
                {
                    firstR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    secondV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    firstV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }

                if(secondR != 0 && firstR != 0)
                {
                    // ERROR ! R*R
                    //cout << "ERROR R*R" << endl;
                    return -1;
                }
                else if(firstR !=0 && secondR == 0)  // R * A
                {
                    relativity.push(secondV*firstR);
                }
                else if(secondR !=0 && firstR == 0)   // A * R
                {
                    relativity.push(firstV*secondR);

                }
                else if(secondR == 0 && firstR == 0)
                {
                    relativity.push(0);
                }



                value.push(firstV*secondV);

            }
            else if(postfix.at(i) == '/')
            {
                sign = true;
                if(!relativity.empty())
                {
                    secondR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!relativity.empty())
                {
                    firstR = relativity.top();
                    relativity.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    secondV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }
                if(!value.empty())
                {
                    firstV = value.top();
                    value.pop();
                }
                else
                {
                    return -1;
                }


                if(secondR == 0 && firstR == 0)
                {
                    // NOT ERROR A/A
                    if(firstV % secondV == 0)
                    {
                        // not a fraction
                        // cout << "Normal division" << endl;

                        if(secondV == 0)
                        {
                            //     cout << "Division By Zero " << endl;
                            return -1;
                        }

                        relativity.push(0);
                        value.push(firstV/secondV);
                    }
                    else
                    {
                        // ERROR a Fraction
                        // cout << "ERROR Fraction" << endl;
                        return -1;
                    }

                }
                else
                {
                    // ERROR ! R/A A/R R/R
                    // cout << "ERROR ! R/A OR A/R OR R/R" << endl;
                    return -1;
                }
            }
            else// if(postfix.at(i) != ' ')
            {
                sign = false;
                temp += postfix.at(i);
                if(postfix.at(i) < '0' || postfix.at(i) > '9')
                {
                    isNumber = false;
                }

            }
        }
        if(!value.empty())
        {
            secondV = value.top();
            value.pop();
        }
        else
        {
            return -1;
        }
        if(!relativity.empty())
        {
            secondR = relativity.top();
            relativity.pop();
        }
        else
        {
            return -1;
        }

        if(secondV < 0)
        {
            // cout << "ERROR NEGATIVE VALUE" << endl;
        }

        if(!value.empty())
        {
            // cout << "Stack not Empty !" << endl;
        }

        if(secondR == 0)
        {
            //  cout << "Absolute Expression" << endl;
            absolute = true;
        }
        else if(secondR == 1)
        {
            // cout << "Relative Expression" << endl;
            absolute = false;
        }
        else
        {
            absolute = false;
            //cout << "\"Neither NOR\" Expression " << endl;
            return -1;
        }

        return secondV;

    }

    void initPassTwo()
    {
        readFile = fopen ("LISAFILE.txt","r");
        writeFile = fopen("LISAFILE.txt","a");
        objFile = fopen("OBJFILE.txt","w");
        char f[100];
        fgets(f,100,readFile);
        string g(f);
        while( g.at(0) != '#' )  // REACH FIRST LINE FOR THE CODE
        {
            g = fgetLine(readFile);
        }
        current = fgetLine(readFile);
        print(writeFile,">>  *****************************************************************************************");
        print(writeFile,">>  S t a r t   o f    P a s s   I I");
        print(writeFile,">>  A s s e m b l e d    p r o g r a m     l i s t i n g");
        string temp = "";
        while ( current.at(0) == ' ' )
        {
            temp += current;
            print(writeFile,current);
            current = fgetLine(readFile);
        }
    }

