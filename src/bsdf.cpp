#include "rdr/bsdf.h"

#include "rdr/fresnel.h"
#include "rdr/interaction.h"
#include "rdr/math_aliases.h"
#include "rdr/platform.h"

RDR_NAMESPACE_BEGIN

static Vec3f obtainOrientedNormal(
    const SurfaceInteraction &interaction, bool twosided) {
  AssertAllValid(interaction.shading.n);
  AssertAllNormalized(interaction.shading.n);
  return twosided && interaction.cosThetaO() < 0 ? -interaction.shading.n
                                                 : interaction.shading.n;
}

/* ===================================================================== *
 *
 * IdealDiffusion
 *
 * ===================================================================== */

void IdealDiffusion::crossConfiguration(
    const CrossConfigurationContext &context) {
  auto texture_name = properties.getProperty<std::string>("texture_name");
  auto texture_ptr  = context.textures.find(texture_name);
  if (texture_ptr != context.textures.end()) {
    texture = texture_ptr->second;
  } else {
    Exception_("Texture [ {} ] not found", texture_name);
  }

  clearProperties();
}

Vec3f IdealDiffusion::evaluate(SurfaceInteraction &interaction) const {
  const Vec3f normal = obtainOrientedNormal(interaction, twosided);
  if (Dot(interaction.wi, normal) < 0 || Dot(interaction.wo, normal) < 0)
    return {0, 0, 0};
  return texture->evaluate(interaction) * INV_PI;
}

Float IdealDiffusion::pdf(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  return 0.0f;
  // UNIMPLEMENTED;
}

Vec3f IdealDiffusion::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *out_pdf) const {
  // This is left as the next assignment
  if (out_pdf != nullptr) *out_pdf = 0.0f;
  return Vec3f(0.0f, 0.0f, 0.0f);
  // UNIMPLEMENTED;
}

/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
  return false;
}

/* ===================================================================== *
 *
 * PerfectRefraction
 *
 * ===================================================================== */

PerfectRefraction::PerfectRefraction(const Properties &props)
    : eta(props.getProperty<Float>("eta", 1.5F)), BSDF(props) {}

Vec3f PerfectRefraction::evaluate(SurfaceInteraction &) const {
  // Since this is a delta distribution, it has no contribution to the queried
  // direction
  return {0.0, 0.0, 0.0};
}

Float PerfectRefraction::pdf(SurfaceInteraction &) const {
  return 0;
}

Vec3f PerfectRefraction::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf) const {
  Vec3f n = interaction.shading.n;
    float cosi = Dot(interaction.wo, n);
    bool entering = cosi > 0;

    float eta_i = entering ? 1.0f : eta;
    float eta_t = entering ? eta : 1.0f;

    Vec3f incident = -interaction.wo;      
    if (!entering) {
        n = -n;           
        cosi = -cosi;
    }

    float eta_ratio = eta_i / eta_t;

    // TODO(HW3): implement the refraction logic here.
  //
  // You should set the `interaction.wi` to the direction of the "in-coming
  // light" after refraction or reflection. Note that `interaction.wi` should
  // always point away from the surface.
  //
  // You may find the following values useful:
  //
  // `interaction.wo`: the out-going view direction, pointing away from the
  // surface.
  // `normal`: the normal of the surface at the interaction point, pointing
  // away from the surface.
  //
  // You may find the following functions useful:
  // @see Refract for refraction calculation.
  // @see Reflect for reflection calculation.

    Vec3f refracted_dir;
    bool ok = Refract(incident, n, eta_ratio, refracted_dir);

    if (!ok) {
        refracted_dir = Reflect(incident, n);
    }

    // wi must be outgoing direction (same convention as BSDF)
    interaction.wi = refracted_dir;

    if (pdf) *pdf = 1.0f;
    return Vec3f(1.0f);
}

bool PerfectRefraction::isDelta() const {
  return true;
}

/* ===================================================================== *
 *
 * FresnelSpecular
 *
 * ===================================================================== */

Glass::Glass(const Properties &props)
    : R(props.getProperty<Vec3f>("R", Vec3f(1.0))),
      T(props.getProperty<Vec3f>("T", Vec3f(1.0))),
      eta(props.getProperty<Float>("eta", 1.5F)),
      BSDF(props) {}

Vec3f Glass::evaluate(SurfaceInteraction &) const {
  // Since this is a delta distribution, it has no contribution to the queried
  // direction
  return {0.0, 0.0, 0.0};
}

Float Glass::pdf(SurfaceInteraction &) const {
  return 0;
}

Vec3f Glass::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf) const {
  // This is left as the next assignment
  if (pdf != nullptr) *pdf = 0.0f; 
  return Vec3f(0.0f, 0.0f, 0.0f);
  // UNIMPLEMENTED;
}

bool Glass::isDelta() const {
  return true;
}

/* ===================================================================== *
 *
 * MicrofacetReflection
 *
 * ===================================================================== */

void MicrofacetReflection::crossConfiguration(
    const CrossConfigurationContext &context) {
  auto texture_name = properties.getProperty<std::string>("texture_name");
  auto texture_ptr  = context.textures.find(texture_name);
  if (texture_ptr != context.textures.end()) {
    R = texture_ptr->second;
  } else {
    Exception_("Texture {} not found", texture_name);
  }

  clearProperties();
}

MicrofacetReflection::MicrofacetReflection(const Properties &props)
    : k(props.getProperty<Vec3f>("k", Vec3f(1.0))),
      etaI(props.getProperty<Vec3f>("etaI", Vec3f(1.0F))),
      etaT(props.getProperty<Vec3f>("etaT", Vec3f(1.0F))),
      dist(props.getProperty<Float>("alpha_x", 0.1),
          props.getProperty<Float>("alpha_y", 0.1)),
      BSDF(props) {}

Vec3f MicrofacetReflection::evaluate(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  return Vec3f(0.0f);
  // UNIMPLEMENTED;
}

Float MicrofacetReflection::pdf(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  return 0.0f;
  // UNIMPLEMENTED;
}

Vec3f MicrofacetReflection::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf_in) const {
  // This is left as the next assignment
  if (pdf_in != nullptr) *pdf_in = 0.0f; 
  return Vec3f(0.0f, 0.0f, 0.0f);
  // UNIMPLEMENTED;
}

/// return whether the bsdf is perfect transparent or perfect reflection
bool MicrofacetReflection::isDelta() const {
  return false;
}

RDR_NAMESPACE_END
