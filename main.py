from manim import *
import numpy as np

class HollowParabola(ThreeDScene):
    def construct(self):
        self.set_camera_orientation(phi= 2 * DEGREES, theta=90 * DEGREES)

        # Parameters
        r_min = 0.4
        r_max = 5.0
        resolution = 32

        # Single parametric function for both positive and negative z
        def hollow_paraboloid(u, v):
            # u ranges from -1 to 1
            r = r_min + (r_max - r_min) * abs(u)  # radial distance is always positive
            theta = 2 * np.pi * v
            x = r * np.cos(theta)
            y = r * np.sin(theta)
            z = (r**2) * np.sign(u)  # z positive if u>0, negative if u<0
            return np.array([x, y, z])

        # Create the surface
        surface = Surface(
            hollow_paraboloid,
            u_range=[-1, 1],
            v_range=[0, 1],
            resolution=(resolution, resolution),
            fill_opacity=0.7,
            checkerboard_colors=[BLUE_D, BLUE_E]
        )

        axes = ThreeDAxes()

        self.add(axes)
        self.play(Create(surface))
        self.wait()
