#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include <concepts>
#include <type_traits>

#include <Eigen/Geometry>

namespace ii {

    using u32 = uint32_t;
    using u64 = uint64_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using f32 = float;
    using f64 = double;
    using byte = std::byte;
    using usize = std::size_t;
    using isize = std::ptrdiff_t;

    inline constexpr usize operator"" _uz(unsigned long long value) noexcept {
        return static_cast<usize>(value);
    }

    template <typename T>
    using Uptr = std::unique_ptr<T>;
    template <typename T>
    using Sptr = std::shared_ptr<T>;

    template <typename T, typename... Args>
    inline auto make_uptr(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T, typename... Args>
    inline auto make_sptr(Args&&... args) {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T>
    using Opt = std::optional<T>;

    template <typename T>
    using Stdvec = std::vector<T>;

    using Vec2d = Eigen::Vector2d;
    using Vec2f = Eigen::Vector2f;
    using Mat2 = Eigen::Matrix2d;
    using Mat3 = Eigen::Matrix3d;
    using Affine2d = Eigen::Affine2d;
    using Translation2d = Eigen::Translation2d;
    using Rotation2d = Eigen::Rotation2Dd;
    using Alignscaling2d = Eigen::AlignedScaling2d;

    template<typename Scalar = f64>
    using Vector2 = Eigen::Matrix<Scalar, 2, 1>;

    template<typename Scalar = f64>
    using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;

    template<typename Scalar = f64>
    using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;

    template<typename Derived>
    using MatrixBase = Eigen::MatrixBase<Derived>;

    template<typename Derived>
    using ArrayBase = Eigen::ArrayBase<Derived>;

    namespace concepts {
        // Base concept: ANY Eigen expression (Matrix or Array)
        template<typename T>
        concept EigenExpression = requires {
            typename T::Scalar;
            { T::RowsAtCompileTime } -> std::convertible_to<int>;
            { T::ColsAtCompileTime } -> std::convertible_to<int>;
            requires std::is_base_of_v<Eigen::EigenBase<T>, T>;
        };

        // Matrix-specific concept (linear algebra)
        template<typename T>
        concept EigenMatrix = EigenExpression<T> && 
            Eigen::internal::traits<T>::IsMatrixAtCompileTime;

        // Array-specific concept (element-wise)
        template<typename T>
        concept EigenArray = EigenExpression<T> &&
            Eigen::internal::traits<T>::IsArrayAtCompileTime;

        // Column vector concept
        template<typename T>
        concept ColumnVector = EigenMatrix<T> && 
            (T::ColsAtCompileTime == 1 || T::ColsAtCompileTime == Eigen::Dynamic);

        // 2D column vector (your primary use case)
        template<typename T>
        concept Vector2D = ColumnVector<T> && 
            (T::RowsAtCompileTime == 2 || T::RowsAtCompileTime == Eigen::Dynamic);

        // 2x2 matrix concept
        template<typename T>
        concept Matrix2D = EigenMatrix<T> && 
            T::RowsAtCompileTime == 2 && T::ColsAtCompileTime == 2;

        // Affine transformation concept
        template<typename T>
        concept AffineTransform2D = requires(const T& t) {
            requires EigenMatrix<T>;
            { t.linear() } -> std::same_as<typename T::LinearMatrixType>;
            { t.translation() } -> ColumnVector;
            requires T::Dim == 2;
        };

    }
    template<typename T>
    concept eigen_expr = ii::concepts::EigenExpression<T>;

    template<typename T>
    concept eigen_matrix = ii::concepts::EigenMatrix<T>;

    template<typename T>
    concept column_vector = ii::concepts::ColumnVector<T>;

    template<typename T>
    concept vec2_like = ii::concepts::Vector2D<T>;

    template<typename T>
    concept affine2_like = ii::concepts::AffineTransform2D<T>;

}