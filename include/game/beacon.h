#ifndef BEACON_H_
#define BEACON_H_

#include "item.h"
class Beacon : public Item {

    public:
        Beacon(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id);

        ~Beacon();
        virtual void Update(double dt) override;
};

#endif 