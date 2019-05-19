#ifndef shader_sources_h
#define shader_sources_h

static const GLchar *solidColor_vertexShaderSource = "\
attribute highp vec2 a_position;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
void main() {\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *solidColor_fragmentShaderSource = "\
uniform mediump vec4 u_color;\
void main() {\
    gl_FragColor = u_color;\
}\
";

static const GLchar *vertexColor_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform mediump vec4 u_color;\
varying mediump vec4 v_color;\
void main() {\
    v_color = a_color * u_color;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *vertexColor_fragmentShaderSource = "\
varying mediump vec4 v_color;\
void main() {\
    gl_FragColor = v_color;\
}\
";

static const GLchar *solidColorWithTexture_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute highp vec2 a_uv0;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
varying highp vec2 v_uv0;\
void main() {\
    v_uv0 = a_uv0;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *solidColorWithTexture_fragmentShaderSource = "\
uniform sampler2D u_texture0;\
uniform mediump vec4 u_color;\
varying highp vec2 v_uv0;\
void main() {\
    gl_FragColor = texture2D(u_texture0, v_uv0) * u_color;\
}\
";

static const GLchar *vertexColorWithTexture_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute highp vec2 a_uv0;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform mediump vec4 u_color;\
varying highp vec2 v_uv0;\
varying mediump vec4 v_color;\
void main() {\
    v_uv0 = a_uv0;\
    v_color = a_color * u_color;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *vertexColorWithTexture_fragmentShaderSource = "\
uniform sampler2D u_texture0;\
varying highp vec2 v_uv0;\
varying mediump vec4 v_color;\
void main() {\
    if (v_uv0.x < -0.1) {\
        gl_FragColor = v_color;\
    } else {\
        gl_FragColor = texture2D(u_texture0, v_uv0) * v_color;\
    }\
}\
";

static const GLchar *pointSprite_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform highp float u_pointSize;\
void main() {\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
    gl_PointSize = u_pointSize;\
}\
";

#endif /* shader_sources_h */
