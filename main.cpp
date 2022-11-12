#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const std::string URL = "https://www.avito.ru/dolgoprudnyy/kvartiry/";
const std::string URL_ADD = "sdam/na_dlitelnyy_srok-ASgBAgICAkSSA8gQ8AeQUg?cd=1";
const int PAGE_NUM = 4;

std::string name_start = "target=\"_blank\" rel=\"noopener\" title=\"";
std::string name_end = "\" itemProp=\"url\" data-marker=\"item-title\"";

std::string price_start = "<meta itemProp=\"price\" content=\"";
std::string price_end = "\"/><meta itemProp=\"availability\" content=\"";

std::string ref_start = "<div class=\"iva-item-titleStep-pdebR\"><a href=\"/dolgoprudnyy/kvartiry/";
std::string ref_end = "\" target=\"_blank\" rel=\"noopener\" title=\"";

void getAvito(std::string url, int pn) {
  std::string cmd;
  for (int pg = 0; pg < pn; ++pg) {
    std::ofstream ofs;
    ofs.open("out_avito_" + std::to_string(pg + 1) + ".txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    cmd = "curl \"" + url + "&p=" + std::to_string(pg + 1) + "\" >> out_avito_" + std::to_string(pg + 1) + ".txt";
    std::cout << cmd << '\n';
    system(cmd.c_str());
  }
}

std::string readAvito(std::string filename) {
  std::ifstream fin;
  fin.open(filename);

  std::stringstream ss;
  ss << fin.rdbuf();
  return ss.str();
}

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::vector<std::string> split(std::string s, std::string start, std::string end) {
  std::vector<std::string> start_split = split(s, start), res;
  for (int idx = 1; idx < start_split.size(); ++idx)
    res.push_back(split(start_split[idx], end)[0]);
  return res;
}

int f(std::string snum, double &num) {
  std::stringstream sstr;
  for (auto val : snum)
    if (val >= '0' && val <= '9' || val == ',')
      sstr << (val == ',' ? '.' : val);
  sstr >> num;
  return 0;
}

int main() {
  //getAvito(URL + URL_ADD, PAGE_NUM);
  std::string content = "";
  for (int pg = 0; pg < PAGE_NUM; ++pg)
    content += readAvito("out_avito_" + std::to_string(pg + 1) + ".txt");

  std::vector<std::string> names = split(content, name_start, name_end), prices = split(content, price_start, price_end), refs = split(content, ref_start, ref_end);

  double pr2m2_min = 2e9;
  std::string ans = "";
  
  for (int idx = 0; idx < names.size(); ++idx) {
    double area = 1.0;
    f(split(names[idx], ", ", " м²,")[0], area);

    double price;
    std::istringstream(prices[idx]) >> price;

    std::cout << names[idx] << ' ' << area << ' ' << price << '\n';

    double pr2m2_cur = price / area;
    if (pr2m2_cur < pr2m2_min) {
      ans = refs[idx];
      pr2m2_min = pr2m2_cur;
    }
  }

  std::cout << URL + ans << '\n';
  return 0;
}