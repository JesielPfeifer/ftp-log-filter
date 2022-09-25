#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <locale>

using namespace std;

class DataHora {
private:
    time_t timestamp;
    time_t data1;
    time_t data2;
    string tmDataUm;
    string tmDataDois;
public:
    DataHora() {}
    DataHora(string timestamp) {
        istringstream issTimestamp(timestamp);
        issTimestamp >> this->timestamp;
    }

    string getDataHora() {
        char buff[20];
        time_t now = this->timestamp;
        strftime(buff, 20, "%Y-%m-%dT%H:%M:%S", localtime(&now));
        return buff;
    }

    time_t getData1() {
        return this->data1;
    }

    time_t getData2() {
        return this->data2;
    }

    void setData1(time_t data1) {
        this->data1 = data1;
    }
    void setData2(time_t data2) {
        this->data2 = data2;
    }

    time_t getTimeStamp() {
        return this->timestamp;
    }

    bool operator== (const DataHora &dados) const {
        return this->data1 == dados.data2;
    }
    bool operator> (const DataHora &dados) const {
        return this->data1 > dados.data2;
    }
    bool operator< (const DataHora &dados) const {
        return this->data1 < dados.data2;
    }
    bool operator>= (const DataHora &dados) const {
        return this->data1 >= dados.data2;
    }
    bool operator<= (const DataHora &dados) const {
        return this->data1 <= dados.data2;
    }

};

class Registro {
private:
    DataHora *dataHora;
    string strDataHora;
    string origemIP;
    int origemPorta;
    string comando;
    string mimeType;
    int fileSize;
    int replyCode;
    string replyMsg;
    bool filtro = true;
    string linha;
public:
    Registro() {};

    Registro(string linha) {
        this->linha = linha;
        string dadoPorta, dadoFileSize, dadoReplyCode;
        istringstream issLinha(linha);

        getline(issLinha, strDataHora, '\t');
        this->dataHora = new DataHora(strDataHora);

        getline(issLinha, origemIP, '\t');

        getline(issLinha, dadoPorta, '\t');
        this->origemPorta = atoi(dadoPorta.c_str());

        getline(issLinha, comando, '\t');

        getline(issLinha, mimeType, '\t');

        getline(issLinha, dadoFileSize, '\t');
        this->fileSize = atoi(dadoFileSize.c_str());

        getline(issLinha, dadoReplyCode, '\t');
        this->replyCode = atoi(dadoReplyCode.c_str());

        getline(issLinha, replyMsg, '\n');
    }
    void setFiltro(bool filtro) {
        this->filtro = filtro;
    }

    bool getFiltro() {
        return this->filtro;
    }

    DataHora *getDataHora() {
        return this->dataHora;
    }
    string getStrDataHora() {
        return this->strDataHora;
    }
    string getOrigemIP() {
        return this->origemIP;
    }
    int getOrigemPorta() {
        return this->origemPorta;
    }

    string getComando() {
        return this->comando;
    }

    string getMimeType() {
        return this->mimeType;
    }

    int getFileSize() {
        return this->fileSize;
    }

    int getReplyCode() {
        return this->replyCode;
    }

    string getReplyMsg() {
        return this->replyMsg;
    }
    string getLinha(){
        return this->linha;
    }
};

class Sistema {
private:
    vector <Registro*> logs;
    string arqLog;
    int portaInicial=-1, portaFinal=-1, fileSizeInicial=-1, fileSizeFinal=-1, replyCode=-1, sbMenu;
    string dataInicial = "", dataFinal = "", ip = "", comando = "", mimeType = "", replyMsg = "";
public:

    Sistema() {}

    ~Sistema() {}

    Sistema(string arqLog) {
        this->arqLog = arqLog;
    }

    void lerArquivo(string arqLog) {
        cout << "Lendo arquivo..." << "[" << logs.size() << "]" << endl;
        fstream arq;
        arq.open(arqLog.c_str(), fstream::in);
        string linha, textTop;
        if(arq.is_open()) {
            getline(arq,textTop);
            if(textTop !="timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg");
            while(!arq.eof()) {
                getline(arq, linha,'\n');
                Registro *registro = new Registro(linha);
                logs.push_back(registro);
            }
        } else {
            cout << "Erro ao abrir" << endl;
        }
        cout << "Leitura do arquivo completa " << "[" << logs.size() << "]" << " registros feitos" << endl;
        cout << endl;
        system("pause");
    }

    void imprimeDadosFiltrados() {
         vector<Registro*> logsValidos = this->getFiltro();
         cout << "timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg\n";
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            cout << (*it)->getDataHora()->getDataHora() << '\t';
            cout << (*it)->getOrigemIP() << '\t';
            cout << (*it)->getOrigemPorta() << '\t';
            cout << (*it)->getComando() << '\t';
            cout << (*it)->getMimeType() << '\t';
            if((*it)->getFileSize() == 0) {
                cout << "-" << '\t';
            } else {
                cout << (*it)->getFileSize() << '\t';
            }
            cout << (*it)->getReplyCode() << '\t';
            cout << (*it)->getReplyMsg() << endl;
        }
    }

    vector<Registro*> getFiltro() {
        vector<Registro*> logsOk;
        for(vector<Registro*>::iterator it = this->logs.begin(); it != this->logs.end(); ++it) {
            if((*it)->getFiltro()) {
                logsOk.push_back(*it);
            }
        }
        return logsOk;
    }
    void limpaFiltros() {
        string confirma;
        do {
            cout << "Deseja limpar os filtros [S][N]: ";
            cin >> confirma;
        } while(confirma != "S" && confirma != "N");
        if(confirma == "S") {
            for(vector<Registro*>::iterator it = logs.begin(); it != logs.end(); ++it) {
                (*it)->setFiltro(true);
            }
            cout << "Filtros excluidos!" << endl;
        } else {
            cout << "Filtros nao foram excluidos!" << endl;
        }
    }

    string serialize() {
        vector<Registro*>logsValidos = this->getFiltro();
        ostringstream oss;
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            oss << (*it)->getOrigemIP() << '\t';
        }

        return oss.str();
    }

    void salvaEmArquivo() {
        string nomeArq;

        cout << "Digite um novo nome para salvar o arquivo: ";
        cin >> nomeArq;

        nomeArq = nomeArq + ".txt";

        vector <Registro*> logsValidos = this->getFiltro();
        fstream arq;
        arq.open(nomeArq.c_str(), fstream::out);
        if(arq.is_open()) {
            cout << "Salvando dados filtrados em novo arquivo..." << endl;
            arq << "timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg\n";
            for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
                arq << (*it)->getDataHora()->getDataHora() << '\t';
                arq << (*it)->getOrigemIP() << '\t';
                arq << (*it)->getOrigemPorta() << '\t';
                arq << (*it)->getComando() << '\t';
                if((*it)->getMimeType() == "") {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getMimeType() << '\t';
                }

                if((*it)->getFileSize() == 0) {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getFileSize() << '\t';
                }

                if((*it)->getReplyCode() == 0) {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getReplyCode() << '\t';
                }
                if((*it)->getReplyMsg() == "") {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getReplyMsg() << '\n';
                }
            }
            arq.close();
        }
            cout << "Arquivos foram salvos." << endl;
    }
    string toUpperCase(string frase) {
        string caixaAlta = "";
        for(int i=0; i<frase.size(); i++) {
            caixaAlta += toupper(frase[i]);
        }
        return caixaAlta;
    }
    void filtroIp(string ip) {
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if((*it)->getOrigemIP() != ip) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroPorta(int portaInicial, int portaFinal) {
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if((*it)->getOrigemPorta() < portaInicial || (*it)->getOrigemPorta() > portaFinal) {
                (*it)->setFiltro(false);
            }
        }
    }
    void filtroComando(string comando) {
        comando = this->toUpperCase(comando);
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if((*it)->getComando() != comando) {
                (*it)->setFiltro(false);
            }
        }
    }
    void filtroMimeType(string mimeType) {
        mimeType = this->toUpperCase(mimeType);
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpperCase((*it)->getMimeType());
            size_t found = str.find(mimeType);
            if(found == string::npos) {
                (*it)->setFiltro(false);
            }
        }
    }
    void filtroFileSize(int fileSizeInicial, int FileSizeFinal) {
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if((*it)->getFileSize() < fileSizeInicial || (*it)->getFileSize() > FileSizeFinal) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyCode (int replyCode) {
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if((*it)->getReplyCode() != replyCode) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyMsg (string replyMensagem) {
        replyMensagem = this->toUpperCase(replyMensagem);
        vector<Registro*> logsValidos = this->getFiltro();
        for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpperCase((*it)->getReplyMsg());
            size_t found = str.find(replyMensagem);
            if(!(found != string::npos)) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtraData(string data1, string data2) {
        struct tm date1 = {};
        struct tm date2 = {};

        string avisoIgual = "ATENCAO! Datas iguais, filtro sera aplicado apenas nos valores informados!\n";
        string avisoData1Menor = "Data Inicial nao pode ser maior que data final\n";
        string error = "Parametros incorretos! Tente novamente\n";

        int flagMenor = -1;
        int flagIgual = -1;
        int flagError = -1;

        bool confirmaData1 = false;
        bool confirmaData2 = false;

        stringstream issData1(data1);
        stringstream issData2(data2);

        time_t dt1;
        time_t dt2;

        if(issData1 >> get_time(&date1,"%Y-%m-%dT%H:%M:%S")) {
            confirmaData1 = true;
            dt1 = mktime(&date1);
            cout << dt1 << endl;
        } else {
            cout << "ERROR! Insira uma data no valor ISO 8601 para filtrar" << endl;
        }
        if(issData2 >> get_time(&date2,"%Y-%m-%dT%H:%M:%S")) {
            confirmaData2 = true;
            dt2 = mktime(&date2);
            cout << dt2 << endl;
        } else {
            cout << "ERROR! Insira uma data no valor ISO 8601 para filtrar" << endl;
        }
        if(confirmaData1 && confirmaData2) {
            vector<Registro*> logsValidos = this->getFiltro();
            for(vector<Registro*>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
                (*it)->getDataHora()->setData1(dt1);
                (*it)->getDataHora()->setData2(dt2);
                if((*it)->getDataHora()->getTimeStamp() < (*it)->getDataHora()->getData1() || (*it)->getDataHora()->getTimeStamp() > (*it)->getDataHora()->getData2()) {
                    (*it)->setFiltro(false);
                }
                if((*it)->getDataHora()->getData1() > (*it)->getDataHora()->getData2()) {
                    flagMenor = 1;
                }
                if((*it)->getDataHora()->getData1() >= (*it)->getDataHora()->getData2()) {
                    flagError = 1;
                }
                if((*it)->getDataHora()->getData1() <= (*it)->getDataHora()->getData2()) {
                    flagError = 1;
                }
                if((*it)->getDataHora()->getTimeStamp() == (*it)->getDataHora()->getData1() && (*it)->getDataHora()->getTimeStamp() == (*it)->getDataHora()->getData2()) {
                    flagIgual = 1;
                }
            }
        }
        if(flagMenor == 1) {
            cout << avisoData1Menor;
        }
        if(flagIgual == 1) {
            cout << avisoIgual;
        }
        if(flagError == 1) {
            cout << error;
        }
        system("pause");
    }
    void filtrosAtivos() {
        cout << "===========================VISUALIZA FILTROS===========================" << endl;
        cout << "Visualize os filtros ativos e nao ativos:" << endl;
        cout << endl;
        if (dataInicial != "") {
            cout << "Data hora inicial: " << dataInicial << endl;
            cout << "Data hora final: " << dataFinal << endl;
        } else {
            cout << "Nenhum filtro de data hora aplicado." << endl;
        }
        if (this->ip != "") {
            cout << "IP: " << ip << endl;
        } else {
            cout << "Nenhum filtro de IP aplicado." << endl;
        }
        if (portaInicial > 0) {
            cout << "Porta inicial: " << portaInicial << endl;
            cout << "Porta final: " << portaFinal << endl;
        } else {
            cout << "Nenhum filtro de porta aplicado" << endl;
        }
        if (comando != "") {
            cout << "Comando: " << comando << endl;
        } else {
            cout << "Nenhum filtro de comando aplicado" << endl;
        }
        if (mimeType != "") {
            cout << "Mime Type: " << mimeType << endl;
        } else {
            cout << "Nenhum filtro de Mime Type aplicado" << endl;
        }
        if (fileSizeInicial > 0) {
            cout << "Tamanho inicial: " << fileSizeInicial << endl;
            cout << "Tamanho final: " << fileSizeFinal << endl;
        } else {
            cout << "Nenhum filtro de File Size aplicado" << endl;
        }
        if (replyCode > 0) {
            cout << "Reply code: " << replyCode << endl;
        } else {
            cout << "Nenhum filtro de Reply Code aplicado" << endl;
        }
        if (replyMsg != "") {
            cout << "Reply Message: " << replyMsg << endl;
        } else {
            cout << "Nenhum filtro de Reply Message aplicado" << endl;
        }
        cout << endl;
    }
    void subMenuFiltros() {
        system("cls");
        cout << "=====================SUB MENU=====================" << endl;
        cout << "Escolha o filtro que deseja adicionar: " << endl;
        cout << "[1] Data e hora" << endl;
        cout << "[2] IP" << endl;
        cout << "[3] Porta" << endl;
        cout << "[4] Comando" << endl;
        cout << "[5] Mime Type" << endl;
        cout << "[6] File Size" << endl;
        cout << "[7] Reply Code" << endl;
        cout << "[8] Reply Message" << endl;
        cout << endl;
        cout << "[0] Voltar ao menu principal" << endl;
        cout << endl;
        cout << "Digite uma opcao [ ]\b\b";
        cin >> sbMenu;
        switch (sbMenu) {
        case 1:
            cout << "Digite a data inicial: ";
            cin >> dataInicial;
            cout << "Digite a data final: ";
            cin >> dataFinal;
            this->filtraData(dataInicial, dataFinal);
            break;
        case 2:
            do {
                cout << "Digite o IP: ";
                cin >> ip;
                if(ip == "") {
                    cout << "IP invalido, insira um IP valido para filtrar" << endl;
                }
            } while(ip == "");
            this->filtroIp(ip);
            break;
        case 3:
            do {
                cout << "Digite o valor da Porta Inicial: ";
                cin >> portaInicial;
                cout << "Digite o valor da Porta Final: ";
                cin >> portaFinal;
                if(portaInicial < 1 || portaFinal < 1 || portaInicial > portaFinal) {
                    cout << "Valores de porta abaixo de zero ou Porta Inicial maior que Porta Final, insira valores validos para filtrar" << endl;
                }
            } while(portaInicial < 1 && portaFinal < 1 && portaInicial > portaFinal);
            this->filtroPorta(portaInicial, portaFinal);
            break;
        case 4:
            do {
                cout << "Digite o comando FTP: ";
                cin >> comando;
                if(comando == "") {
                    cout << "Comando invalido, insira um Comando valido para filtrar" << endl;
                }
            } while(comando == "");
            this->filtroComando(comando);
            break;
        case 5:
            do {
                cout << "Informe o MimeType que deseja filtrar: ";
                cin >> mimeType;
                if(mimeType == "") {
                    cout << "MimeType invalido, insira um valor valido para filtrar" << endl;
                }
            } while(mimeType == "");
            this->filtroMimeType(mimeType);
            break;
        case 6:
            do {
                cout << "Informe o intervalo inicial do File Size que deseja filtrar: ";
                cin >> fileSizeInicial;
                cout << "Informe o intervalo final do File Size que deseja filtrar: ";
                cin >> fileSizeFinal;
                if(fileSizeInicial <= 0 || fileSizeFinal <=0) {
                    cout << "Valor invalido,digite novamente" << endl;
                }
            } while(fileSizeInicial <=0 || fileSizeFinal <=0);
            this->filtroFileSize(fileSizeInicial,fileSizeFinal);

            break;
        case 7:
            do {
                cout << "Informe o Reply Code que deseja filtrar: ";
                cin >> replyCode;
                if(replyCode < 0) {
                    cout << "Valor invalido, digite novamente" << endl;
                }
            } while(replyCode < 0);
            this->filtroReplyCode(replyCode);
            break;
        case 8:
            do {
                cout << "Informe a Reply Message que deseja filtrar: ";
                getline(cin,replyMsg);
                if(replyMsg == "") {
                    cout << "Expressao invalida,digite novamente" << endl;
                }
            }  while(replyMsg == "");
            this->filtroReplyMsg(replyMsg);
            break;
        }
    }
};

int main() {
    string arquivo = "ftp.log", nomeArq;
    Sistema *sistema = new Sistema();
    sistema->lerArquivo(arquivo);
    int opc;
    while(1) {
        system("cls");
        cout << "=========================MENU=========================" << endl;
        cout << "1 - Adicionr filtro" << endl;
        cout << "2 - Limpar filtros" << endl;
        cout << "3 - Visualizar filtros" << endl;
        cout << "4 - Visualizar dados" << endl;
        cout << "5 - Exportar dados" << endl;
        cout << "0 - Fechar programa" << endl;
        cout << "Digite uma opcao [ ]\b\b";
        cin >> opc;
        switch (opc) {
        case 1:
            system("cls");
            sistema->subMenuFiltros();
            break;
        case 2:
            system("cls");
            sistema->limpaFiltros();
            break;
        case 3:
            system("cls");
            sistema->filtrosAtivos();
            system("pause");
            break;
        case 4:
            system("cls");
            sistema->imprimeDadosFiltrados();
            system("pause");
            break;
        case 5:
            system("cls");
            sistema->salvaEmArquivo();
            system("pause");
            break;
        case 0:
            return 0;
        default:
            break;
        }
    }
    return 0;
}
