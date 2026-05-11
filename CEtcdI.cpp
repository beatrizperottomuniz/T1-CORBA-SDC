#include "CEtcdI.h"
#include <iostream>

using namespace std;

// -------------------------------------------------------
// Construtor: inicializa o identificador do objeto
// -------------------------------------------------------
CEtcd_i::CEtcd_i()
{
    identif = "CEtcd-objeto-1";
    cout << "[servidor] CEtcd_i criado com id = " << identif << "\n";
}

// -------------------------------------------------------
// Destrutor
// -------------------------------------------------------
CEtcd_i::~CEtcd_i()
{
}

// -------------------------------------------------------
// id(): retorna o identificador do objeto
// -------------------------------------------------------
std::string CEtcd_i::id()
{
    cout << "[servidor] id() -> " << identif << "\n";
    return identif;
}

// -------------------------------------------------------
// put(key, val):
//   - insere ou atualiza chave na tabela
//   - retorna TRUE  se a chave é NOVA
//   - retorna FALSE se a chave JÁ EXISTIA (valor foi atualizado)
// -------------------------------------------------------
::CORBA::Boolean CEtcd_i::put(const std::string& key, const std::string& val)
{
    bool nova = (tabela.find(key) == tabela.end());
    tabela[key] = val;

    if (nova) {
        cout << "[servidor] put(\"" << key << "\", \"" << val << "\") -> chave NOVA (true)\n";
    } else {
        cout << "[servidor] put(\"" << key << "\", \"" << val << "\") -> chave ATUALIZADA (false)\n";
    }

    return nova;   // true = nova, false = já existia
}

// -------------------------------------------------------
// get(key):
//   - retorna o valor associado à chave
//   - lança InvalidKey se a chave não existir
// -------------------------------------------------------
std::string CEtcd_i::get(const std::string& key)
{
    auto it = tabela.find(key);
    if (it == tabela.end()) {
        cout << "[servidor] get(\"" << key << "\") -> chave NAO ENCONTRADA (InvalidKey)\n";
        throw InvalidKey();
    }
    cout << "[servidor] get(\"" << key << "\") -> \"" << it->second << "\"\n";
    return it->second;
}

// -------------------------------------------------------
// del(key):
//   - remove a chave e seu valor da tabela
//   - lança InvalidKey se a chave não existir
// -------------------------------------------------------
void CEtcd_i::del(const std::string& key)
{
    auto it = tabela.find(key);
    if (it == tabela.end()) {
        cout << "[servidor] del(\"" << key << "\") -> chave NAO ENCONTRADA (InvalidKey)\n";
        throw InvalidKey();
    }
    tabela.erase(it);
    cout << "[servidor] del(\"" << key << "\") -> removida com sucesso\n";
}
