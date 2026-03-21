#include <GraphEngine/Graphs/GraphObjects/ThreeDAxes.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Arrow3D.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/2D/Text.hpp>

ThreeDAxes::ThreeDAxes(TConfig config) {
    this->graph_func = nullptr;
    this->config = config;
    this->resolution = {100, 32};
}

void ThreeDAxes::generatePoints() {
    for(auto child : subGraphObjects) {
        // delete child? Depends on ownership. GraphMathObject doesn't have a virtual destructor shown.
    }
    subGraphObjects.clear();
    points.clear();
    fill_indices.clear();
    stroke_indices.clear();
    sub_surface_ranges.clear();

    float tick_size = 0.5f;

    Arrow3D *x_arrow = new Arrow3D(glm::vec3(x_range.first, 0, 0), glm::vec3(x_range.second + config.head_length, 0, 0), config.shaft_radius, config.head_length);
    x_arrow->setStrokeColors({GraphColor(1, 0, 0)});
    x_arrow->setFillColors({config.axis_color});
    x_arrow->showFill = true;
    x_arrow->showStroke = false;
    this->add(x_arrow);

    // Y-Axis (Green)

    Arrow3D* y_arrow = new Arrow3D(glm::vec3(0, y_range.first, 0), glm::vec3(0, y_range.second + config.head_length, 0), config.shaft_radius, config.head_length);
    y_arrow->showStroke = false;
    y_arrow->setStrokeColors({GraphColor(0, 1, 0)});
    y_arrow->setFillColors({config.axis_color});
    y_arrow->showFill = true;
    this->add(y_arrow);

    // Z-Axis (Blue)
    Arrow3D* z_arrow = new Arrow3D(glm::vec3(0, 0, z_range.first), glm::vec3(0, 0, z_range.second + config.head_length), config.shaft_radius, config.head_length);
    z_arrow->setStrokeColors({GraphColor(0, 0, 1)});
    z_arrow->setFillColors({config.axis_color});
    z_arrow->showFill = true;
    z_arrow->showStroke = false;
    this->add(z_arrow);

    // 2. Add Labels
    Text* x_label = new Text("x", x_range.second +config.head_length, 0, 0, config.axis_label_size);
    x_label->setStrokeColors({GraphColor(1, 0, 0)});
    this->add(x_label);

    Text* y_label = new Text("y", 0, y_range.second +config.head_length, 0, config.axis_label_size);
    y_label->setStrokeColors({GraphColor(0, 1, 0)});
    this->add(y_label);

    Text* z_label = new Text("z", 0, 0, config.head_length, config.axis_label_size);
    // z_label->setTranslate(glm::vec3(0, 0, z_range.second + 0.8f));
    z_label->setStrokeColors({GraphColor(0, 0, 1)});
    this->add(z_label);

    // 3. Add Ticks
    // X ticks
    for (float i = ceil(x_range.first); i <= floor(x_range.second); i += 1.0f) {
        if (abs(i) < 0.001f) continue;
        int idx = points.size();
        points.push_back(glm::vec3(i, -tick_size, 0));
        points.push_back(glm::vec3(i, tick_size, 0));
        stroke_indices.push_back(idx);
        stroke_indices.push_back(idx + 1);
    }
    // Y ticks
    for (float i = ceil(y_range.first); i <= floor(y_range.second); i += 1.0f) {
        if (abs(i) < 0.001f) continue;
        int idx = points.size();
        points.push_back(glm::vec3(-tick_size, i, 0));
        points.push_back(glm::vec3(tick_size, i, 0));
        stroke_indices.push_back(idx);
        stroke_indices.push_back(idx + 1);
    }
    // Z ticks
    for (float i = ceil(z_range.first); i <= floor(z_range.second); i += 1.0f) {
        if (abs(i) < 0.001f) continue;
        int idx = points.size();
        points.push_back(glm::vec3(0, -tick_size, i));
        points.push_back(glm::vec3(0, tick_size, i));
        stroke_indices.push_back(idx);
        stroke_indices.push_back(idx + 1);
    }

    if (!points.empty()) {
        sub_surface_ranges.push_back({0, (int)points.size()});
    }
    updateDimensions();
}

void ThreeDAxes::Init() {
    ThreeDObject::Init();
}
