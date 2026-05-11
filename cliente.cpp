//
// CLIENTE CEtcd — testa todas as operações do objeto distribuído
//
//   Sistemas Distribuídos e Concorrentes
//   Escola Politécnica -- PUCPR
//
// Uso via IOR:
//   ./cliente <IOR-string>        (passa IOR diretamente)
//   ./cliente -f cetcd.ior        (lê IOR de arquivo)
//   ./cliente -ns                 (busca pelo Name Service)
//

#include <iostream>
#include <fstream>
#include <string>
#include <CEtcdC.h>

// Para usar o Serviço de Nomes
#include <orbsvcs/CosNamingC.h>

using namespace std;
using namespace CORBA;

// Utilitário: lê IOR de um arquivo
static string lerIOR(const string& filename) {
    ifstream f(filename);
    if (!f) {
        cerr << "Erro: nao foi possivel abrir " << filename << "\n";
        exit(1);
    }
    string ior;
    f >> ior;
    return ior;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <IOR>       (string IOR direta)\n";
        cerr << "     " << argv[0] << " -f <arquivo> (IOR em arquivo)\n";
        cerr << "     " << argv[0] << " -ns           (busca no Name Service)\n";
        return 1;
    }

    try {
        // 1. Inicializa o ORB
        ORB_var orb = ORB_init(argc, argv, "ORB");

        // 2. Obtém referência para o objeto distribuído
        CEtcd_var etcd;

        string modo = argv[1];

        if (modo == "-f") {
            // Lê IOR de arquivo
            if (argc < 3) { cerr << "Faltou nome do arquivo.\n"; return 1; }
            string ior = lerIOR(argv[2]);
            Object_ptr ref = orb->string_to_object(ior.c_str());
            etcd = CEtcd::_narrow(ref);

        } else if (modo == "-ns") {
            // Busca no Name Service
            Object_ptr ns_obj = orb->resolve_initial_references("NameService");
            CosNaming::NamingContext_var ns =
                CosNaming::NamingContext::_narrow(ns_obj);

            CosNaming::Name nome;
            nome.length(1);
            nome[0].id   = CORBA::string_dup("CEtcd");
            nome[0].kind = CORBA::string_dup("");

            Object_ptr ref = ns->resolve(nome);
            etcd = CEtcd::_narrow(ref);

        } else {
            // Trata argv[1] como IOR direta
            Object_ptr ref = orb->string_to_object(argv[1]);
            etcd = CEtcd::_narrow(ref);
        }

        if (CORBA::is_nil(etcd.in())) {
            cerr << "Erro: nao foi possivel obter referencia para CEtcd.\n";
            return 1;
        }

        // ========================================================
        // 3. TESTA TODAS AS OPERAÇÕES
        // ========================================================

        cout << "\n========================================\n";
        cout << "  TESTE DO OBJETO DISTRIBUIDO CEtcd\n";
        cout << "========================================\n\n";

        // --- id() ---
        cout << "[TESTE] id()\n";
        string identificador = etcd->id();
        cout << "  Resultado: \"" << identificador << "\"\n\n";

        // --- put() com chave nova ---
        cout << "[TESTE] put(\"host\", \"192.168.0.1\")  -- chave nova\n";
        bool result = etcd->put("host", "192.168.0.1");
        cout << "  Resultado: " << (result ? "true (chave nova)" : "false (atualizada)") << "\n\n";

        cout << "[TESTE] put(\"port\", \"8080\")  -- chave nova\n";
        result = etcd->put("port", "8080");
        cout << "  Resultado: " << (result ? "true (chave nova)" : "false (atualizada)") << "\n\n";

        cout << "[TESTE] put(\"env\", \"producao\")  -- chave nova\n";
        result = etcd->put("env", "producao");
        cout << "  Resultado: " << (result ? "true (chave nova)" : "false (atualizada)") << "\n\n";

        // --- put() com chave existente (deve retornar false) ---
        cout << "[TESTE] put(\"port\", \"9090\")  -- chave JA EXISTENTE\n";
        result = etcd->put("port", "9090");
        cout << "  Resultado: " << (result ? "true (chave nova)" : "false (atualizada)") << "\n\n";

        // --- get() com chave existente ---
        cout << "[TESTE] get(\"host\")  -- chave existente\n";
        string val = etcd->get("host");
        cout << "  Resultado: \"" << val << "\"\n\n";

        cout << "[TESTE] get(\"port\")  -- deve retornar valor atualizado\n";
        val = etcd->get("port");
        cout << "  Resultado: \"" << val << "\"  (esperado: 9090)\n\n";

        // --- get() com chave inexistente (deve lançar InvalidKey) ---
        cout << "[TESTE] get(\"chave_inexistente\")  -- deve lancar InvalidKey\n";
        try {
            val = etcd->get("chave_inexistente");
            cout << "  ERRO: deveria ter lancado InvalidKey!\n\n";
        } catch (const InvalidKey&) {
            cout << "  Resultado: InvalidKey lancada corretamente!\n\n";
        }

        // --- del() com chave existente ---
        cout << "[TESTE] del(\"env\")  -- chave existente\n";
        etcd->del("env");
        cout << "  Resultado: removida com sucesso\n\n";

        // --- get() após del (deve lançar InvalidKey) ---
        cout << "[TESTE] get(\"env\")  -- apos del, deve lancar InvalidKey\n";
        try {
            val = etcd->get("env");
            cout << "  ERRO: deveria ter lancado InvalidKey!\n\n";
        } catch (const InvalidKey&) {
            cout << "  Resultado: InvalidKey lancada corretamente!\n\n";
        }

        // --- del() com chave inexistente (deve lançar InvalidKey) ---
        cout << "[TESTE] del(\"chave_que_nao_existe\")  -- deve lancar InvalidKey\n";
        try {
            etcd->del("chave_que_nao_existe");
            cout << "  ERRO: deveria ter lancado InvalidKey!\n\n";
        } catch (const InvalidKey&) {
            cout << "  Resultado: InvalidKey lancada corretamente!\n\n";
        }

        cout << "========================================\n";
        cout << "  TODOS OS TESTES CONCLUIDOS\n";
        cout << "========================================\n\n";

        // 4. Finaliza ORB
        orb->destroy();

    } catch (const Exception& e) {
        cerr << "ERRO CORBA: " << e << "\n";
        return 1;
    }

    return 0;
}
