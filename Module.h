#ifndef MODULE_H_
#define MODULE_H_

#include <vector>

#include "Interaction.h"
#include "Uniform.h"

/*
* This is the base class for modules.
* Any future feature of this graphics framework should extend Module.
* Modules are automatically enabled for user interactions.
*/
class Module {
public:
	Module() {}
	virtual ~Module() {}
	virtual void interact(Interaction* i) {}
	virtual Interaction* interaction_type() { return new Interaction(); }
	virtual std::vector<Uniform> uniforms() { return std::vector<Uniform>{}; }

	virtual void pre_pass() {} //TODO let world call this to render in pre pass mode
};

#endif