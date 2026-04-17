#ifdef _MSC_VER
#define NOMINMAX
#endif

#define TEST_NO_MAIN
#include "acutest.h"

#include "unit-pprint.h"
#include "prim-types.hh"
#include "value-types.hh"
#include "value-pprint.hh"
#include "pprinter.hh"
#include "usdShade.hh"
#include "usdGeom.hh"
#include "usdLux.hh"

using namespace tinyusdz;

void value_type_pprint_test(void) {

  {
    std::stringstream ss;
    tinyusdz::Interpolation interp = tinyusdz::Interpolation::Vertex;
    ss << interp;
    TEST_CHECK(ss.str() == "vertex");
  }

  {
    value::normal3f v{1.0f, 2.0f, 3.f};
    std::string s = to_string(v);
    TEST_CHECK(s == "(1, 2, 3)");
  }
}

void pprint_uvtexture_wrap_test(void) {
  UsdUVTexture tex;
  tex.wrapS.set_value(UsdUVTexture::Wrap::Repeat);
  tex.wrapT.set_value(UsdUVTexture::Wrap::Clamp);

  std::string result = to_string(tex);

  TEST_CHECK_(result.find("inputs:wrapS = \"repeat\"") != std::string::npos,
              "wrapS should map to inputs:wrapS with value repeat");
  TEST_CHECK_(result.find("inputs:wrapT = \"clamp\"") != std::string::npos,
              "wrapT should map to inputs:wrapT with value clamp");

  TEST_CHECK_(result.find("inputs:wrapS = \"clamp\"") == std::string::npos,
              "wrapS must not contain clamp (would indicate swap)");
  TEST_CHECK_(result.find("inputs:wrapT = \"repeat\"") == std::string::npos,
              "wrapT must not contain repeat (would indicate swap)");
}

void pprint_bool_attr_test(void) {
  {
    GeomMesh mesh;
    mesh.doubleSided.set_value(true);

    std::string result = to_string(mesh);

    TEST_CHECK_(result.find("doubleSided = true") != std::string::npos,
                "bool true should serialize as 'true', not '1'");
    TEST_CHECK_(result.find("doubleSided = 1") == std::string::npos,
                "bool should not serialize as numeric '1'");
  }

  // false-case regression — guards against asymmetric boolalpha bugs
  {
    GeomMesh mesh;
    mesh.doubleSided.set_value(false);

    std::string result = to_string(mesh);

    TEST_CHECK_(result.find("doubleSided = false") != std::string::npos,
                "bool false should serialize as 'false', not '0'");
    TEST_CHECK_(result.find("doubleSided = 0") == std::string::npos,
                "bool should not serialize as numeric '0'");
  }
}

void pprint_bool_animatable_attr_test(void) {
  // Covers TypedAttributeWithFallback<Animatable<bool>> — a separate printer
  // overload from TypedAttributeWithFallback<bool> (GeomMesh::doubleSided).
  // Real consumers: UsdLux SphereLight/CylinderLight `inputs:normalize` and
  // `inputs:enableColorTemperature`, UsdSkel `collection:*:includeRoot`.
  {
    SphereLight light;
    light.normalize.set_value(true);

    std::string result = to_string(light);

    TEST_CHECK_(result.find("inputs:normalize = true") != std::string::npos,
                "Animatable<bool> true should serialize as 'true'");
    TEST_CHECK_(result.find("inputs:normalize = 1") == std::string::npos,
                "Animatable<bool> must not serialize as numeric '1'");
  }

  {
    SphereLight light;
    light.normalize.set_value(false);

    std::string result = to_string(light);

    TEST_CHECK_(result.find("inputs:normalize = false") != std::string::npos,
                "Animatable<bool> false should serialize as 'false'");
    TEST_CHECK_(result.find("inputs:normalize = 0") == std::string::npos,
                "Animatable<bool> must not serialize as numeric '0'");
  }
}

void pprint_uvtexture_st_type_test(void) {
  // Fallback (set_value) form
  {
    UsdUVTexture tex;
    tex.st.set_value(value::float2{0.5f, 0.5f});

    std::string result = to_string(tex);

    TEST_CHECK_(result.find("float2 inputs:st") != std::string::npos,
                "inputs:st should be typed as float2 per USD spec");
    TEST_CHECK_(result.find("texcoord2f inputs:st") == std::string::npos,
                "inputs:st should not use texcoord2f type");
  }

  // Connection form — the actual usdchecker failure case
  // (UsdShade Sdr-compliance: Expected 'float2' on UsdUVTexture.inputs:st).
  {
    UsdUVTexture tex;
    tex.st.set_connection(Path("/Root/Mat/StReader", "outputs:result"));

    std::string result = to_string(tex);

    TEST_CHECK_(result.find("float2 inputs:st.connect") != std::string::npos,
                "inputs:st.connect must declare float2 (UsdShade Sdr-compliance)");
    TEST_CHECK_(result.find("texcoord2f inputs:st.connect") == std::string::npos,
                "inputs:st.connect must not declare texcoord2f");
  }
}
