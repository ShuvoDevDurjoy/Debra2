from manim import *
import numpy as np

class MyScene(Scene):
    def construct(self):
        axes = Axes(
            x_range=[-4, 4],
            y_range=[-4, 4],
            axis_config={"include_numbers": False}
        )

        self.add(axes)

        x = np.linspace(-3, 3, 300)

        def graph(f, color):
            g = VMobject(color=color)
            g.set_points_smoothly([axes.c2p(t, f(t)) for t in x])
            return g

        g1 = graph(lambda t: t*t, RED)
        g2 = graph(lambda t: np.sin(t), BLUE)
        g3 = graph(lambda t: t**3 / 4, GREEN)
        g4 = graph(lambda t: np.exp(t) / 5, YELLOW)

        self.play(Create(g1))
        self.wait(0.5)

        self.play(Transform(g1, g2), run_time=2)
        self.play(Transform(g1, g3), run_time=2)
        self.play(Transform(g1, g4), run_time=2)

        self.wait()
