//
// SERVIDOR CEtcd — objeto distribuído de chave/valor
//
//   Sistemas Distribuídos e Concorrentes
//   Escola Politécnica -- PUCPR
//

#include <iostream>
#include <fstream>
#include <string>
#include "CEtcdI.h"

// Para usar o Serviço de Nomes (Name Service / CosNaming)
#include <orbsvcs/CosNamingC.h>

using namespace std;
using namespace CORBA;
using namespace PortableServer;

int main(int argc, char* argv[])
{
    try {
        // 1. Inicia o ORB
        ORB_var orb = ORB_init(argc, argv, "ORB");

        // 2. Ativa o RootPOA
        Object_ptr aux = orb->resolve_initial_references("RootPOA");
        POA_var poa = POA::_narrow(aux);
        POAManager_var ger = poa->the_POAManager();
        ger->activate();

        // 3. Instancia o servant (implementação)
        CEtcd_i servant;

        // 4. Registra o servant no POA → cria o objeto distribuído
        CEtcd_var cetcd = servant._this();

        // 5a. Também salva a IOR em arquivo (fallback)
        string ior = orb->object_to_string(cetcd.in());
        ofstream("cetcd.ior") << ior;
        cout << "IOR salva em cetcd.ior\n";

        // 5b. Publica no Serviço de Nomes (Name Service)
        //     O cliente pode buscar pelo nome "CEtcd"
        try {
            Object_ptr ns_obj = orb->resolve_initial_references("NameService");
            CosNaming::NamingContext_var ns =
                CosNaming::NamingContext::_narrow(ns_obj);

            CosNaming::Name nome;
            nome.length(1);
            nome[0].id   = CORBA::string_dup("CEtcd");
            nome[0].kind = CORBA::string_dup("");

            // rebind: atualiza se já existir, evita erro de conflito
            ns->rebind(nome, cetcd.in());
            cout << "Objeto registrado no Name Service como \"CEtcd\"\n";

        } catch (const Exception& e) {
            cerr << "AVISO: Nao foi possivel registrar no Name Service: "
                 << e << "\n";
            cerr << "       Use a IOR do arquivo cetcd.ior para o cliente.\n";
        }

        // 6. Aguarda requisições
        cout << "Servidor aguardando requisicoes...\n";
        orb->run();

        // 7. Finalizações
        cout << "Servidor encerrando.\n";
        poa->destroy(true, true);
        orb->destroy();

    } catch (const Exception& e) {
        cerr << "ERRO CORBA: " << e << "\n";
        return 1;
    }

    return 0;
}
