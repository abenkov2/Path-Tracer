Path Tracer Part IV: Global Illumination
======================

**University of Pennsylvania, CIS 561: Advanced Computer Graphics, Homework 7**

Name: Ari Benkov
------------
PennKey: 64120499
------------

Results:
------------
Custom Scene 1: 2 Orbs (1 Glass + 1 Metallic) + Glass Disc, 20x20 samples, recursion depth = 8

![](https://github.com/CIS-461-2021/homework-07-global-illumination-abenkov2/blob/master/test_renders/final_renders/2OrbsGlassDisc.png)

Custom Scene 2: 3 Glass Orbs, 20x20 samples, recursion depth = 8

![](https://github.com/CIS-461-2021/homework-07-global-illumination-abenkov2/blob/master/test_renders/final_renders/3GlassOrbs.png)

Custom Scene 3: Nested Glass Orbs with Sphere Lights, 30x30 samples, recursion depth = 8

![](https://github.com/CIS-461-2021/homework-07-global-illumination-abenkov2/blob/master/test_renders/final_renders/NestdOrbsWithSphereLights_30Samples.png)


Overview
------------
You will implement a new integrator class: `FullLightingIntegrator`. This
integrator will compute both the direct lighting and global illumination at
each ray intersection to produce a more converged image in a shorter period of
time. Compare the result of using the `NaiveIntegrator` to render the
two-light Cornell Box scene to the result of using the `FullLightingIntegrator`

![](./twolightsNaive.png) ![](./twolightsFull.png)

Both scenes were rendered with 100 samples per pixel and maximum 5 iterations.
Notice how the `FullLightingIntegrator` result is much less noisy; this is
because each ray path is much more likely to receive light from a light source
as each intersection directly samples a light.

Useful Reading
---------
Once again, you will find the textbook will be very helpful when implementing
this homework assignment. We recommend referring to the following chapters:
* 14.5: Path Tracing

The Light Transport Equation
--------------
#### L<sub>o</sub>(p, &#969;<sub>o</sub>) = L<sub>e</sub>(p, &#969;<sub>o</sub>) + &#8747;<sub><sub>S</sub></sub> f(p, &#969;<sub>o</sub>, &#969;<sub>i</sub>) L<sub>i</sub>(p, &#969;<sub>i</sub>) V(p', p) |dot(&#969;<sub>i</sub>, N)| _d_&#969;<sub>i</sub>

* __L<sub>o</sub>__ is the light that exits point _p_ along ray &#969;<sub>o</sub>.
* __L<sub>e</sub>__ is the light inherently emitted by the surface at point _p_
along ray &#969;<sub>o</sub>.
* __&#8747;<sub><sub>S</sub></sub>__ is the integral over the sphere of ray
directions from which light can reach point _p_. &#969;<sub>o</sub> and
&#969;<sub>i</sub> are within this domain.
* __f__ is the Bidirectional Scattering Distribution Function of the material at
point _p_, which evaluates the proportion of energy received from
&#969;<sub>i</sub> at point _p_ that is reflected along &#969;<sub>o</sub>.
* __L<sub>i</sub>__ is the light energy that reaches point _p_ from the ray
&#969;<sub>i</sub>. This is the recursive term of the LTE.
* __V__ is a simple visibility test that determines if the surface point _p_' from
which &#969;<sub>i</sub> originates is visible to _p_. It returns 1 if there is
no obstruction, and 0 is there is something between _p_ and _p_'. This is really
only included in the LTE when one generates &#969;<sub>i</sub> by randomly
choosing a point of origin in the scene rather than generating a ray and finding
its intersection with the scene.
* The __absolute-value dot product__ term accounts for Lambert's Law of Cosines.

Updating this README (5 points)
-------------
Make sure that you fill out this `README.md` file with your name and PennKey,
along with your test renders. You should render each of the new scenes we have
provided you, once with each integrator type. At minimum we expect renders using
the default sample count and recursion depth, but you are encouraged to try
rendering scenes with more samples to get nicer looking results.

Possibly moving your old direct lighting code
---------
If you implemented the multiple importance sampling assignment in
`FullLightingIntegrator`, please move all of that code into
`DirectLightingIntegrator`, as you will implement the global illumination
path tracer in `FullLightingIntegrator`. That being said, you will end up
re-using some of your multiple importance sampling code for this assignment.

An iterative implementation of `FullLightingIntegrator::Li` (15 points)
--------------
In order to more easily track certain pieces of information, such as the
cumulative throughput of your ray path and whether or not your ray just bounced
off a specular surface, we are going to implement the `FullLightingIntegrator`'s
`Li` function using a `while` loop rather than having it invoke itself
recursively. To begin, your `Li` should instantiate an accumulated ray color
that begins as black, and an accumulated ray throughput color that begins as
white. The throughput will be used to determine when your ray path terminates
via the Russian Roulette heuristic. Next, simply declare a `while` loop that
compares some current depth value to 0, assuming that depth began as the maximum
depth value. Within this loop, we will add a check that breaks the loop early
if the Russian Roulette conditions are satisfied. Just after this `while` loop,
your `Li` code should return the accumulated ray color, which will be computed
within the loop.

Computing the direct lighting component (20 points)
----------
Using the same code as you wrote for the previous assignment, use
multiple importance sampling to evaluate the direct lighting your ray
intersection receives from a randomly chosen light source. This value will be
__one__ term; if your BSDF-generated ray does not hit the light source you
randomly chose, then its contribution will be black. This term should NOT
include any global illumination light, or any light from any other light source.

As in your `DirectLightingIntegrator`, make sure to scale the direct lighting
contribution by the number of light sources in your scene; since you are
sampling each light source at a frequency inversely proportional to the number
of lights in the scene, you must scale each light's contribution upward to
counteract its otherwise lessened contribution.

Similarly to your `DirectLightingIntegrator`, we are going to treat the direct
lighting contribution of specular surfaces as black. We perform a slightly
less costly BSDF contribution computation below, so we're just going to ignore
the direct lighting term entirely when it comes to specular materials.

Note that your direct lighting computation should incorporate the entire
light transport equation and evaluate the color of the surface as lit by
direct illumination. Make sure to store this value separately from your overall
accumulated color.

Computing the ray bounce and global illumination (15 points)
-------
Separately from your direct lighting term, you should use an entirely new
2D uniform random variable to generate a new BSDF-based &#969;<sub>i</sub> using `Sample_f`. This ray will be used to determine the direction of your
ray bounce and global illumination computation.
You will multiply the color obtained from `Sample_f` with
your ray throughput, along with the absolute-dot-product term and the 1/_pdf_
term. This effectively compounds the inherent material colors of all surfaces
this ray has bounced from so far, so that when we incorporate the lighting
this particular ray bounce receives _directly_, it is attenuated by __all__ of
the surfaces our ray has previously hit. To this end, once you have your
updated throughput, multiply it with the direct lighting term from above and
add the result to your accumulated ray color. Finally, make sure to update the
ray that your `while` loop is using to originate from your current
intersection and travel in the direction of the &#969;<sub>i</sub> that you
just computed.

Correctly accounting for direct lighting (10 points)
-------
Since your path tracer computes the direct lighting a given intersection
receives as its own term, your path tracer must __not__ include _too much_
light. This means that every ray which already computed the direct lighting term
should __not__ incorporate the `Le` term of the light transport equation into
its light contribution. In other words, unless a particular ray came directly
from the camera or from a perfectly specular surface, `Le` should be ignored.

Russian Roulette Ray Termination (10 points)
-------
After you compute the direct lighting and indirect lighting terms,
compare the maximum RGB component of your throughput to a uniform random number
and stop your `while` loop if said component is smaller than the random number.
Most importantly, to keep your path tracer mathematically correct, you must then divide your throughput by the maximum component of your throughput; this scales up the contribution of this ray path proportionally to how likely the path is to terminate so that we are still accounting for the contribution that paths that _were_ terminated earlier would have had.

Custom scenes (30 points)
-----------
Since you are only implementing a new integrator this week, you will have time
available to design and render scenes of your own. Please create and
render __at least three__ unique scenes. They should not be variations on the
Cornell box! We want you to have unique material for your demo reel (and some
first-hand experience working with the JSON format we have made for scenes).
We expect each scene to be rendered with a __minimum of 20x20 samples per
pixel__ and with a __hard recursion limit of 8 bounces__ (though your Russian
Roulette may stop it earlier). Additionally, each image should be a __minimum
of 512x512 pixels__, but at least one larger image is recommended for better
demo reel quality.

We ask that your custom scenes showcase the more visually impressive components
of your path tracer, such as a scene that includes a microfacet mirror surface
and several glass materials of various indices of refraction. Additionally,
texture maps, normal maps, and microfacet roughness maps are encouraged. You may
try to render scenes containing triangle meshes, but bear in mind that they will
take a while if your meshes have a high number of faces.

Extra credit (30 points maximum)
-----------
Choose any extra credit from a previous path tracer homework assignment and
implement it, provided of course that you did not previously do so. As always,
you are free to implement a feature not listed as extra credit if it is not a
required feature of some other assignment, e.g. a subsurface scattering BXDF.

Submitting your project
--------------
Along with your project code, make sure that you fill out this `README.md` file
with your name and PennKey, along with your test renders.

Rather than uploading a zip file to Canvas, you will simply submit a link to
the committed version of your code you wish us to grade. If you click on the
__Commits__ tab of your repository on Github, you will be brought to a list of
commits you've made. Simply click on the one you wish for us to grade, then copy
and paste the URL of the page into the Canvas submission form.
