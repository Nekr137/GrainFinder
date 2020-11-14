#include "Slic.h"
#include "ImageRGB.h"
#include <iostream>
#include <algorithm>

namespace SLIC
{

	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	// iX0, iY0 - start poin, iX1, iY1 - end point;
	void Slic::FindLine(int iX0, int iY0, int iX1, int iY1, std::vector<int>& oX, std::vector<int>& oY) {

		int dx = std::abs(iX1 - iX0);
		int dy = std::abs(iY1 - iY0);

		bool reversed = dy > dx;

		if (reversed) {
			std::swap(iX0, iY0);
			std::swap(iX1, iY1);
			std::swap(dx, dy);
		}

		if (iX0 > iX1) {
			std::swap(iX0, iX1);
			std::swap(iY0, iY1);
		}

		double er = 0.0;
		double der = (dy + 1.0) / (dx + 1.0);
		int y = iY0;
		int diry = iY1 - iY0 > 0 ? 1 : -1;

		for (int x = iX0; x <= iX1; ++x) {
			oX.push_back(x);
			oY.push_back(y);
			er += der;
			if (er >= 1.0) {
				y += diry;
				er -= 1.0;
			}
		}

		if (reversed)
			std::swap(oX, oY);
	}

	double Slic::LineGradient(const Cluster& iCl, const P2D& iPixel) {

		auto findUsualSlicDist = [&](const P2D iPix) {
			double dlab = FindColorDist(iCl, iPix);
			double dxy = FindEuqlDist(iCl, iPix);
			return dlab + _m / _S * dxy;
		};

		std::vector<int> xx, yy;
		FindLine((int)iCl.x, (int)iCl.y, iPixel.x, iPixel.y, xx, yy);

		size_t n = xx.size();

		double sum = 0.0, max = 0.0;
		for (size_t i = 0; i < n; ++i) {
			P2D pix(xx[i], yy[i]);
			double dist = findUsualSlicDist(pix);
			sum += dist;
			if (max < dist) max = dist;
		}
		
		// max / mean
		return max / sum * static_cast<double>(n);
	}

	Slic::Slic(const ImageRGB* ipImage)
		: _pImage(ipImage) {

		_imgW = _pImage->GetWidth();
		_imgH = _pImage->GetHeight();
	}

	double Slic::FindColorDist(const Cluster& iCl, const P2D& iPixel) {
		double r, g, b;
		_pImage->Get(iPixel.x, iPixel.y, r, g, b);
		double ll = iCl.l - r;
		double aa = iCl.a - g;
		double bb = iCl.b - b;
		double dlab = std::sqrt(ll*ll + aa*aa + bb*bb);
		return dlab;
	}

	double Slic::FindEuqlDist(const Cluster& iCl, const P2D& iPixel) {
		double xx = iCl.x - iPixel.x;
		double yy = iCl.y - iPixel.y;
		double dxy = std::sqrt(xx*xx + yy*yy);
		return dxy;
	}

	double Slic::ClusterToPixelDist(const Cluster& iCl, const P2D& iPixel) {
		double dlab = FindColorDist(iCl, iPixel);
		double dxy = FindEuqlDist(iCl, iPixel);
		double ds = dlab     +     _m / _S * dxy;
		return ds;
	}

	bool Slic::Perform(const size_t K, const double m) {

		std::cout << "\n [ SLIC ] Performing a SLIC clasterization";

		_m = m;
		size_t N = _imgW * _imgH;
		_S = std::sqrt(N / K);
		size_t seDist = 2.0 * _S;

		double sCount = 2.0;

		std::cout << "\n [ SLIC ] N: " << N;
		std::cout << "\n [ SLIC ] K: " << K;
		std::cout << "\n [ SLIC ] S = sqrt(N/k): " << _S;
		std::cout << "\n [ SLIC ] imgW: " << _imgW;
		std::cout << "\n [ SLIC ] imgH: " << _imgH;
		std::cout << "\n [ SLIC ] imgW/S: " << _imgW / _S;

		// Init clusters
		for (double h = _S;; h += _S) {

			if ((size_t)h > _imgH - _S)
				break;

			for (double w = _S;; w += _S) {

				if ((size_t)w > _imgW - _S)
					break;

				double r, g, b;

				size_t i = static_cast<size_t>(w);
				size_t j = static_cast<size_t>(h);

				_pImage->Get(i, j, r, g, b);
				Cluster cl(r, g, b, (double)i, (double)j);
				_aClusters.push_back(cl);

				// to do: Move
			}
		}

		size_t nClusters = _aClusters.size();

		std::vector<double> distances(_imgW * _imgH * nClusters);
		std::vector<double> distancesFixed(_imgW * _imgH * nClusters);
		std::vector<size_t> pixelClusterIndices(_imgW * _imgH);

		const size_t slicIterationsCnt = 4;
		for (size_t iter = 0; iter < 4; ++iter) {

			std::cout << "\n [ SLIC ] iter: " << iter << " of " << slicIterationsCnt;
			// 
			for (auto& cl : _aClusters)
				cl._aPixels.clear();

			// x, y, clusterIdx
			std::cout << "\n [ SLIC ]\t Find all distances from pixels to clusters";
			for (size_t k = 0; k < _aClusters.size(); ++k) {
				const Cluster& cl = _aClusters[k];
				double stX = std::max(cl.x - sCount * _S, 0.0);
				double stY = std::max(cl.y - sCount * _S, 0.0);
				double enX = std::min(cl.x + sCount * _S, _imgW - 1.0);				
				double enY = std::min(cl.y + sCount * _S, _imgH - 1.0);
				for (size_t i = static_cast<size_t>(stX); i < static_cast<size_t>(enX); ++i) {
					for (size_t j = static_cast<size_t>(stY); j < static_cast<size_t>(enY); ++j) {
						const P2D px(i, j);
						distances[i*j*k] = ClusterToPixelDist(cl, px);
					}
				}
			}

			std::cout << "\n [ SLIC ]\t Take borders into account";
			distancesFixed = distances;
			for (size_t k = 0; k < _aClusters.size(); ++k) {

				const Cluster& cl = _aClusters[k];

				double stX = std::max(cl.x - sCount * _S, 0.0);
				double stY = std::max(cl.y - sCount * _S, 0.0);
				double enX = std::min(cl.x + sCount * _S, _imgW - 1.0);
				double enY = std::min(cl.y + sCount * _S, _imgH - 1.0);

				for (int i = static_cast<int>(stX); i < static_cast<int>(enX); ++i) {
					for (int j = static_cast<int>(stY); j < static_cast<int>(enY); ++j) {

						std::vector<int> xx, yy;
						FindLine((int)cl.x, (int)cl.y, i, j, xx, yy);

						size_t n = xx.size();

						double sum = 0.0, max = 0.0;
						for (size_t m = 0; m < n; ++m) {
							double dist = distances[xx[m] * yy[m] * k];
							sum += dist;
							if (max < dist) max = dist;
						}

						// additive = max / mean
						double additive = max / sum * static_cast<double>(n);
						distancesFixed[i * j * k] += additive;
					}
				}
			}

			std::cout << "\n [ SLIC ]\t Compare all distances";
			for (size_t k = 0; k < _aClusters.size(); ++k) {
				const Cluster& cl = _aClusters[k];
				double stX = std::max(cl.x - sCount * _S, 0.0);
				double stY = std::max(cl.y - sCount * _S, 0.0);
				double enX = std::min(cl.x + sCount * _S, _imgW - 1.0);
				double enY = std::min(cl.y + sCount * _S, _imgH - 1.0);
				for (size_t i = static_cast<size_t>(stX); i < static_cast<size_t>(enX); ++i) {
					for (size_t j = static_cast<size_t>(stY); j < static_cast<size_t>(enY); ++j) {

						double dist = distancesFixed[i * j * k];
						if (pixelClusterIndices[i * j] < dist)
							pixelClusterIndices[i * j] = dist;
					}
				}
			}


			std::cout << "\n [ SLIC ]\t Find cluster centers";
			for (size_t k = 0; k < _aClusters.size(); ++k) {

				Cluster& cl = _aClusters[k];
				double sumR = 0.0, sumG = 0.0, sumB = 0.0, x = 0.0, y = 0.0;
				double nPixels = static_cast<double>(cl._aPixels.size());

				for (const P2D px : cl._aPixels) {

					size_t i = static_cast<size_t>(px.x);
					size_t j = static_cast<size_t>(px.y);

					double r, g, b;
					_pImage->Get(i, j, r, g, b);

					x += px.x;
					y += px.y;
					sumR += r;
					sumG += g;
					sumB += b;
				}

				cl.x = x / nPixels;
				cl.y = y / nPixels;
				cl.l = sumR / nPixels;
				cl.a = sumG / nPixels;
				cl.b = sumB / nPixels;
			}

			std::cout << "\n [ SLIC ]\t Remove weird clusters";
			for (size_t k = _aClusters.size(); k > 0; --k) {

				size_t w, h;
				_aClusters[k - 1].GetSize(w, h);

				bool remove = false;

				//remove |= _aClusters[k - 1]._aPixels.size() < _S;
				//remove |= w > 3 * _S || h > 3 * _S;

				if (remove)
					_aClusters.erase(_aClusters.begin() + k - 1);

			}
		}

		return true;
	}

	void Slic::Show() const {

		std::cout << "\n [ SLIC ] Showing an output";

		// 2D array of cluster indices _imgW x _imgH
		std::vector<std::vector<size_t>> aaCI(_imgW, std::vector<size_t>(_imgH));

		for (size_t clIdx = 0; clIdx < _aClusters.size(); ++clIdx) {
			const Cluster& cl = _aClusters[clIdx];
			for (size_t i = 0; i < cl._aPixels.size(); ++i) {
				const P2D px = cl._aPixels[i];
				aaCI[px.x][px.y] = clIdx;
			}
		}

		Channel ch(_imgW * 2, _imgH);
		ImageRGB im(ch, ch, ch);

		for (size_t i = 0; i < _imgW; ++i) {
			for (size_t j = 0; j < _imgH; ++j) {
				double r, g, b;
				_pImage->Get(i, j, r, g, b);
				im.Set(i, j, r, g, b);
			}
		}

		for (const Cluster& cl : _aClusters) {
			im.Set((size_t)cl.x, (size_t)cl.y, 1.0, 0.0, 0.0);
			if (cl.x > 0) im.Set((size_t)(cl.x - 1), (size_t)cl.y, 1.0, 0.0, 0.0);
			if (cl.y > 0) im.Set((size_t)cl.x, (size_t)(cl.y - 1), 1.0, 0.0, 0.0);
		}

		for (size_t i = 0; i < _imgW; ++i){
			for (size_t j = 0; j < _imgH; ++j){

				if (
					(i > 0 && aaCI[i][j] != aaCI[i - 1][j]) ||
					(i + 1 < _imgW && aaCI[i][j] != aaCI[i + 1][j]) ||
					(j > 0 && aaCI[i][j] != aaCI[i][j - 1]) ||
					(j + 1 < _imgH && aaCI[i][j] != aaCI[i][j + 1])
					) {
					im.Set(i, j, 1.0, 0.0, 0.0);
				}
			}
		}

		size_t nClusters = _aClusters.size();
		std::cout << "\nClusters: " << nClusters;

		auto showGrains = [&]() {
			size_t i = 0;
			for (double r = 0.0; r < 1.0 + 1e-5; r += 0.2) {
				for (double g = 0.0; g < 1.0 + 1e-5; g += 0.2) {
					for (double b = 0.0; b < 1.0 + 1e-5; b += 0.2) {

						if (i == nClusters)
							return;

						for (const auto& px : _aClusters[i]._aPixels) {
							im.Set(px.x + _imgW, px.y, r, g, b);
						}

						i++;
					}
				}
			}
		};

		showGrains();
		im.Show();
	}

	bool Cluster::IsBorderPixel(size_t pxlIdx) const {
		P2D px = _aPixels[pxlIdx];
		size_t neighbCnt = 0;
		for (size_t j = 0; j < _aPixels.size(); ++j) {
			if (neighbCnt == 4)
				break;
			if (pxlIdx == j)
				continue;
			if (px.y == _aPixels[j].y) {
				if (px.x == _aPixels[j].x - 1 || px.x == _aPixels[j].x + 1)
					neighbCnt++;
			}
			if (px.x == _aPixels[j].x) {
				if (px.y == _aPixels[j].y - 1 || px.y == _aPixels[j].y + 1)
					neighbCnt++;
			}
		}
		return neighbCnt < 4;
	}

	void Cluster::GetSize(size_t& oW, size_t& oH) const {

		if (_aPixels.empty())
			return;

		size_t w[2] = { (size_t)x, (size_t)x };
		size_t h[2] = { (size_t)y, (size_t)y };
		for (const P2D px : _aPixels) {
			w[0] = std::min(w[0], px.x);
			w[1] = std::max(w[1], px.x);
			h[0] = std::min(h[0], px.y);
			h[1] = std::max(h[1], px.y);
		}
		oW = w[1] - w[0];
		oH = h[1] - h[0];
	}

};


