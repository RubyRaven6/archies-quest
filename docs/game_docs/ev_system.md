### The Basic Idea
You get a certain number of EVs depending on your level. Each time you level up, you gain a number EVs according to the chart below:

<table>
    <tr>
        <td>Pokémon<br>Levels</td>
        <td>EVs Gained<br>Per Level</td>
        <td>EVs at this/these<br>level/s</td>
    </tr>
    <tr>
        <td>1 - 9</td>
        <td>9</td>
        <td>9</td>
    </tr>
    <tr>
        <td>10</td>
        <td>5</td>
        <td>14</td>
    </tr>
    <tr>
        <td>11 - 19</td>
        <td>9</td>
        <td>23</td>
    </tr>
    <tr>
        <td>20</td>
        <td>5</td>
        <td>28</td>
    </tr>
    <tr>
        <td>21 - 29</td>
        <td>9</td>
        <td>37</td>
    </tr>
    <tr>
        <td>30</td>
        <td>5</td>
        <td>42</td>
    </tr>
    <tr>
        <td>31 - 39</td>
        <td>9</td>
        <td>51</td>
    </tr>
    <tr>
        <td>40</td>
        <td>5</td>
        <td>56</td>
    </tr>
    <tr>
        <td>41 - 49</td>
        <td>9</td>
        <td>65</td>
    </tr>
    <tr>
        <td>50</td>
        <td>5</td>
        <td>70</td>
    </tr>
    <tr>
        <td>51 - 59</td>
        <td>9</td>
        <td>79</td>
    </tr>
    <tr>
        <td>60</td>
        <td>5</td>
        <td>84</td>
    </tr>
    <tr>
        <td>61 - 69</td>
        <td>9</td>
        <td>93</td>
    </tr>
    <tr>
        <td>70</td>
        <td>5</td>
        <td>98</td>
    </tr>
    <tr>
        <td>71 - 100</td>
        <td>30</td>
        <td>128</td>
    </tr>
    <tr>
        <td>TOTAL</td>
        <td colspan=2>128</td>
    </tr>
</table>

EVs are no longer assigned per Pokémon. Each time you level up, you gain more EVs and can assign the EVs you gained to any stat you want. 

In addition, the numbers regarding EVs have been changed. The max EVs a Pokémon can have is 128, while the max it can have in one stat is 63.

### Relevant Calculations
#### HP formula
$$
\lfloor((2 * baseHP + hpIV) * level) / 100)\rfloor + level + (10 + hpEV))
$$
#### Other stats formula
$$
\lfloor(((2 * baseStat + iv) * level) / 100)\rfloor + (5 + ev)
$$
