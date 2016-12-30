#include "ParticipatingMediaToth.h"

ParticipatingMediaToth::ParticipatingMediaToth() {
	n_ = 100;
	P_ = 30000.0f;
	density_ = 0.01f;
	albedo_ = 0.9;

	g_ = 0.5;

	scaling_ = 2.0f;
}

ParticipatingMediaToth::~ParticipatingMediaToth() {

}

std::vector<Uniform> ParticipatingMediaToth::uniforms() {
	return {
		Uniform("Toth_n", [this](Uniform* u, Model* m) {
			u->update(n_);
		}),
		Uniform("Toth_P", [this](Uniform* u, Model* m) {
			u->update(P_);
		}),
		Uniform("Toth_density", [this](Uniform* u, Model* m) {
			u->update(density_);
		}),
		Uniform("Toth_albedo", [this](Uniform* u, Model* m) {
			u->update(albedo_);
		}),
		Uniform("HG_g", [this](Uniform* u, Model* m) {
			u->update(g_);
		}),
		Uniform("Toth_scaling", [this](Uniform* u, Model* m) {
			u->update(scaling_);
		})
	};
}




void ParticipatingMediaToth::interact(Interaction* i) {
	ParticipatingMediaTothInteraction* interaction = (ParticipatingMediaTothInteraction*)i;
	if(interaction->change_uniform == interaction->change_n) {
		interaction->increase ? n_ += 1 : n_ -= 1;
	} else if(interaction->change_uniform == interaction->change_P) {
		interaction->increase ? P_ += 100.0f : P_ -= 100.0f;
	} else if(interaction->change_uniform == interaction->change_albedo) {
		interaction->increase ? albedo_ += 0.1f : albedo_ -= 0.1f;
	} else if(interaction->change_uniform == interaction->change_density) {
		interaction->increase ? density_ += 0.001f : density_ -= 0.001f;
	} else if(interaction->change_uniform == interaction->change_g) {
		interaction->increase ? g_ += 0.01f : g_ -= 0.01f;
	} else if(interaction->change_uniform == interaction->change_scale) {
		interaction->increase ? scaling_ += 0.1f : scaling_ -= 0.1f;
	}
	printf("n_ = %d, P_ = %f, albedo_ = %f, density_ = %f, g_ = %f, scale_ = %f\n",
		n_, P_, albedo_, density_, g_, scaling_);
}

Interaction* ParticipatingMediaToth::interaction_type() {
	return new ParticipatingMediaTothInteraction();
}