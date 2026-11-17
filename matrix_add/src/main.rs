use std::io::{self, Write};

fn main() {
    // 1. 행렬의 크기(행, 열) 입력받기
    print!("행렬의 행(row)과 열(col) 크기를 공백으로 구분해 입력하세요: ");
    io::stdout().flush().unwrap();

    let mut size_input = String::new();
    io::stdin().read_line(&mut size_input).expect("입력 오류");
    let dims: Vec<usize> = size_input
        .split_whitespace()
        .map(|s| s.parse().expect("양의 정수를 입력해야 합니다."))
        .collect();

    if dims.len() != 2 {
        println!("행과 열, 두 개의 숫자를 정확히 입력해주세요.");
        return;
    }

    let rows = dims[0];
    let cols = dims[1];

    // 2. 입력받은 크기만큼 동적으로 행렬(Vec<Vec<i32>>) 할당 및 원소 입력
    println!("\n[첫 번째 행렬 입력]");
    let matrix_a = read_matrix(rows, cols);

    println!("\n[두 번째 행렬 입력]");
    let matrix_b = read_matrix(rows, cols);

    // 3. 두 행렬 더하기 (결과 저장용 Vec 동적 할당)
    let mut result_matrix: Vec<Vec<i32>> = Vec::with_capacity(rows);

    for i in 0..rows {
        let mut row_vec = Vec::with_capacity(cols);
        for j in 0..cols {
            row_vec.push(matrix_a[i][j] + matrix_b[i][j]);
        }
        result_matrix.push(row_vec);
    }

    // 4. 결과 출력
    println!("\n[두 행렬의 합 결과]");
    for row in &result_matrix {
        for val in row {
            print!("{:4} ", val);
        }
        println!();
    }
}

// 행렬 원소를 입력받아 Vec<Vec<i32>> 형태로 반환하는 함수
fn read_matrix(rows: usize, cols: usize) -> Vec<Vec<i32>> {
    let mut matrix: Vec<Vec<i32>> = Vec::with_capacity(rows);

    for i in 0..rows {
        loop {
            print!("{}번째 행의 원소 {}개를 공백으로 구분해 입력: ", i + 1, cols);
            io::stdout().flush().unwrap();

            let mut row_input = String::new();
            io::stdin().read_line(&mut row_input).expect("입력 오류");

            let row: Result<Vec<i32>, _> = row_input
                .split_whitespace()
                .map(|s| s.parse::<i32>())
                .collect();

            match row {
                Ok(r) if r.len() == cols => {
                    matrix.push(r);
                    break;
                }
                _ => println!("정수 {}개를 정확히 다시 입력해주세요.", cols),
            }
        }
    }
    matrix
}
