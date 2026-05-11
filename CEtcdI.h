#ifndef CETCDI_H_
#define CETCDI_H_

#include "CEtcdS.h"
#include <map>
#include <string>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class CEtcd_i : public virtual POA_CEtcd {
public:
    // Construtor
    CEtcd_i();

    // Destrutor
    virtual ~CEtcd_i();

    // Atributo id (somente leitura)
    virtual std::string id();

    // Insere ou atualiza chave/valor.
    // Retorna true se chave nova, false se atualizada.
    virtual ::CORBA::Boolean put(const std::string& key, const std::string& val);

    // Retorna o valor da chave. Lança InvalidKey se não existir.
    virtual std::string get(const std::string& key);

    // Remove chave. Lança InvalidKey se não existir.
    virtual void del(const std::string& key);

private:
    std::string identif;                    // identificador do objeto
    std::map<std::string, std::string> tabela;  // tabela de chave/valor
};

#endif /* CETCDI_H_ */
